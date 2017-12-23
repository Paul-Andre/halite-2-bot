#include "hlt/hlt.hpp"
#include <cassert>
#include <cmath>
#include "hlt/navigation.hpp"
#include "hlt/constants.hpp"

#define SQR(x) ((x)*(x))


bool defending = false;
bool fleeing = false;

int main() {

    const hlt::Metadata metadata = hlt::initialize("bot 17 (back to same)");
    const hlt::PlayerId player_id = metadata.player_id;

    const hlt::Map& initial_map = metadata.initial_map;


    // We now have 1 full minute to analyse the initial map.
    std::ostringstream initial_map_intelligence;
    initial_map_intelligence
            << "width: " << initial_map.map_width
            << "; height: " << initial_map.map_height
            << "; players: " << initial_map.ship_map.size()
            << "; my ships: " << initial_map.ship_map.at(player_id).size()
            << "; planets: " << initial_map.planets.size();
    hlt::Log::log(initial_map_intelligence.str());



    int time =0;

    for (;;) {
        time ++;

        std::vector<hlt::Move> moves;

        hlt::Map map = hlt::in::get_map();

        int my_ships = map.ships[player_id].size();
        int max_opponent_ships = 0;

        for (auto& player_ship : map.ships) {
            if (player_ship.first != player_id) {
                max_opponent_ships = std::max(max_opponent_ships, (int) player_ship.second.size());
            }
        }

        double ratio = (double)my_ships / (double) max_opponent_ships;

        if (defending && ratio > 1.2) {
            defending = false;
        }

        if (
                (max_opponent_ships > 20 && !defending && ratio < 0.9 )
        )
        {
            defending = true;
        }


        if (max_opponent_ships > 75 && ratio < 0.15) {
            fleeing = true;
            for ( hlt::Ship& ship : map.ships.at(player_id)) {
                if (ship.docking_status == hlt::ShipDockingStatus::Docked) {
                    moves.push_back(hlt::Move::undock(ship.entity_id));
                }
            }
        }


        if (fleeing) {
            for ( hlt::Ship& ship : map.ships.at(player_id)) {
                if (ship.docking_status != hlt::ShipDockingStatus::Undocked) {
                    continue;
                }

                int dir = ship.entity_id % 2;
                dir = dir*2 - 1;

                const int max_corrections = 180;
                const bool avoid_obstacles = true;
                const double angular_step_rad = M_PI / 180.0;
                double r = ship.entity_id + time*0.1*dir;

                const hlt::Location& target = 
                    ship.location +
                    ((hlt::Location){ std::cos(r ),
                      std::sin(r) }) * 40;

                hlt::possibly<hlt::Move> move =
                    hlt::navigation::navigate_ship_towards_target(
                            map, ship, target,  7, avoid_obstacles, max_corrections, angular_step_rad, true);

                if (!move.second) {
                    move = 
                        hlt::navigation::navigate_ship_towards_target(
                                map, ship, target,  7, avoid_obstacles, max_corrections, angular_step_rad, false);
                }

                if (move.second) {
                    moves.push_back(move.first);
                    double angle_rad  = move.first.move_angle_deg * (M_PI * 2. /360.);
                    double thrust = move.first.move_thrust;
                    ship.thrust.pos_x = cos(angle_rad) * thrust;
                    ship.thrust.pos_y = sin(angle_rad) * thrust;
                    continue;
                }
            }

        if (!hlt::out::send_moves(moves)) {
            hlt::Log::log("send_moves failed; exiting");
            break;
        }
            continue;
        }

                




        //for (const auto& myShip : map.ships[player_id]) {

        for (auto& player_ship : map.ships) {
            if (player_ship.first != player_id) {
                for(auto& ship : player_ship.second) {
                    for (auto& planet : map.planets) {
                        if (planet.owner_id == player_id) {
                            for (auto ship_id : planet.docked_ships) {
                                hlt::Ship& my_ship = map.get_ship(player_id, ship_id);

                                ship.distance_to_my_closest_planet  = std::min(
                                        ship.distance_to_my_closest_planet,
                                        my_ship.location.get_distance_to(ship.location)
                                );
                            }
                        }
                    }
                }
            }
        }
                            

        for ( hlt::Ship& ship : map.ships.at(player_id)) {
            if (ship.docking_status != hlt::ShipDockingStatus::Undocked) {
                continue;
            }

            double min_so_far = 1e20;


            hlt::Planet *target_planet_ptr = nullptr;

            for( int i = 0; i<map.planets.size(); i++) {


                hlt::Planet &planet = map.planets[i];

                double docked = planet.docked_ships.size();

                if (planet.owner_id == player_id || !planet.owned) {
                    docked += planet.targetted * 0.666;
                }
                else {
                    docked -= planet.targetted * 0.333;
                }

                int remaining_docks = planet.docking_spots - docked;


                double distance = ship.location.get_distance_to(planet.location);

                //distance -= planet.radius;

                double weight = distance;

                if (planet.owned && planet.owner_id != player_id) {
                    weight += docked*4./7. + weight*docked*(1./12.)*4;
                }
                else {
                    weight *= 1 + ((double)docked/planet.docking_spots);
                }

                if (planet.owner_id != player_id || !planet.is_full()){
                    if (weight < min_so_far) {
                        target_planet_ptr = &map.planets[i];
                        min_so_far = weight;
                    }
                }
            }

            auto it = map.ships.begin();


            /*
            // get the best docked ship to molest

            for(; it != map.ships.end(); it++) {
                if(it->first != player_id) {

                    std::vector<hlt::Ship> &v = it->second;
                    for(int i=0; i<v.size(); i++) {
                        double distance = ship.location.get_distance_to(enemy.location);
                        hlt::Ship &enemy = v[i];

                        if (enemy.docking_status == hlt::ShipDockingStatus::Docked
                                */


            double distance_to_target_planet = 10000;
            if (target_planet_ptr != nullptr) {
                hlt::Planet &planet = *target_planet_ptr;
                distance_to_target_planet = planet.location.get_distance_to(ship.location);
            }



            hlt::Ship *ship_ptr = nullptr;

            for(; it != map.ships.end(); it++) {
                if(it->first != player_id) {

                    std::vector<hlt::Ship> &v = it->second;
                    for(int i=0; i<v.size(); i++) {

                        hlt::Ship &enemy = v[i];

                        double distance = ship.location.get_distance_to(enemy.location);

                        if (enemy.docking_status == hlt::ShipDockingStatus::Undocked
                                || enemy.docking_status == hlt::ShipDockingStatus::Undocking){

                            distance /= 5;
                            double weight = distance * distance;
                            int k = enemy.targetted;

                            if (distance_to_target_planet != 10000) {
                                weight *= 0.7*(enemy.distance_to_my_closest_planet)/ distance_to_target_planet;
                            }


                            while(k!=0) {
                                if(defending) {
                                    weight *= 0.7*(enemy.distance_to_my_closest_planet)/ distance_to_target_planet;
                                }
                                weight *= 1.5;
                                k--;
                            }


                            if (weight < min_so_far) {
                                min_so_far = weight;
                                ship_ptr = &v[i];
                            }
                        }
                    }
                }
            }


            if (ship_ptr != nullptr) {
                const hlt::possibly<hlt::Move> move =
                    hlt::navigation::navigate_ship_to_dock(map, ship, *ship_ptr, hlt::constants::MAX_SPEED);

                if (move.second) {
                    moves.push_back(move.first);
                    ship_ptr -> targetted ++;
                        double angle_rad  = move.first.move_angle_deg * (M_PI * 2. /360.);
                        double thrust = move.first.move_thrust;
                        ship.thrust.pos_x = cos(angle_rad) * thrust;
                        ship.thrust.pos_y = sin(angle_rad) * thrust;
                    continue;
                }
            }



            if (target_planet_ptr == nullptr) continue;
            hlt::Planet &planet = *target_planet_ptr;

            planet.targetted ++;

            if (planet.owner_id == player_id || !planet.owned) {

                if (ship.can_dock(planet)) {
                    moves.push_back(hlt::Move::dock(ship.entity_id, planet.entity_id));
                }
                else {
                    const hlt::possibly<hlt::Move> move =
                        hlt::navigation::navigate_ship_to_dock(map, ship, planet, hlt::constants::MAX_SPEED);
                    if (move.second) {
                        moves.push_back(move.first);
                        double angle_rad  = move.first.move_angle_deg * (M_PI * 2. /360.);
                        double thrust = move.first.move_thrust;
                        ship.thrust.pos_x = cos(angle_rad) * thrust;
                        ship.thrust.pos_y = sin(angle_rad) * thrust;
                    }
                }
            }

            else {


                double min_so_far = 1e20;
                int best_i = 0;
                for(int i=0; i<planet.docked_ships.size(); i++) {
                    auto enemy = map.get_ship(planet.owner_id, planet.docked_ships[i]);
                    double distance = ship.location.get_distance_to(enemy.location);
                    distance += (double)(enemy.health)/64. * 7;
                    if (distance < min_so_far) {
                        best_i = i;
                        min_so_far = distance;
                    }
                }

                auto enemy = map.get_ship(planet.owner_id, planet.docked_ships[best_i]);

                const hlt::possibly<hlt::Move> move =
                    hlt::navigation::navigate_ship_to_dock(map, ship, enemy, hlt::constants::MAX_SPEED);

                if (move.second) {
                    moves.push_back(move.first);
                        double angle_rad  = move.first.move_angle_deg * (M_PI * 2. /360.);
                        double thrust = move.first.move_thrust;
                        ship.thrust.pos_x = cos(angle_rad) * thrust;
                        ship.thrust.pos_y = sin(angle_rad) * thrust;
                        enemy.targetted++;
                }
            }

        }

        if (!hlt::out::send_moves(moves)) {
            hlt::Log::log("send_moves failed; exiting");
            break;
        }

    }
}
