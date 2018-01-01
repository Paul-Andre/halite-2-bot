#include "hlt/hlt.hpp"
#include <cassert>
#include <vector>
#include <cmath>
#include "hlt/navigation.hpp"
#include "hlt/constants.hpp"
#include "hlt/gaussian.hpp"
#include <utility>

using std::pair;
using std::vector;

using namespace hlt;
using namespace std;

#define SQR(x) ((x)*(x))


bool defending = false;
bool fleeing = false;
bool rushing = false;

bool always_rushing = true;
bool naive_rushing = false;

Ship rusher;

vector<bool> is_planet_in_center;

#define close_std_dev 5.
#define medium_std_dev 15.


        int number_going_to_center = 0;
        
bool should_rush(Map &map) {


    if (always_rushing) return true;

    int max_targetted = 0;

    for(int i=0; i<map.planets.size(); i++) {
        max_targetted = max(max_targetted, map.planets[i].targetted);
    }

    return map.ships.size() == 2 && ( 
    number_going_to_center >= 2 || (number_going_to_center == 1 && max_targetted == 1) );
}



int main() {

    srand(time(nullptr));

    const hlt::Metadata metadata = hlt::initialize("post 22");
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


    {


        // find the planets that are in the center

        const vector<Planet> &planets = initial_map.planets;
        is_planet_in_center = vector<bool>(planets.size()*2, false); //*2 for no reason at all
        vector<pair<double, EntityId> > v;

        Location center = { initial_map.map_width * .5 , initial_map.map_height * .5 };

        for(int i=0; i< planets.size(); i++) {
            v.push_back(make_pair((center - planets[i].location).norm(), planets[i].entity_id ));
        }

        sort(v.begin(), v.end());

        for(int i=0; i<4; i++) {
            is_planet_in_center[v[i].second] = true;
        }

    }



    int time =0;

    for (;;) {
        time ++;

        std::vector<hlt::Move> moves;

        hlt::Map map = hlt::in::get_map();





        // Count ships and decide modes


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


        if (max_opponent_ships > 75 && ratio < 0.15) 
        {
            fleeing = true;
            for ( hlt::Ship& ship : map.ships.at(player_id)) {
                if (ship.docking_status == hlt::ShipDockingStatus::Docked) {
                    moves.push_back(hlt::Move::undock(ship.entity_id));
                }
            }
        }




        // Calculate gradients!!!!

        for (pair<const hlt::PlayerId, vector<hlt::Ship>>& player_ships_1 : map.ships) {


            vector<Ship> &ships_1 = player_ships_1.second;

            for(int i=0; i<ships_1.size(); i++) {


                hlt::Ship &ship_1 = ships_1[i];


                for (pair<const hlt::PlayerId, vector<hlt::Ship>>& player_ships_2 : map.ships) {

                    
                    vector<Ship> &ships_2 = player_ships_2.second;
                    for(int j=0; j<ships_2.size(); j++) {


                        hlt::Ship &ship_2= ships_2[j];

                        if (ship_2.docking_status == hlt::ShipDockingStatus::Undocked) {
                            

                                Location vec = ship_1.location - ship_2.location;

                                pair<double, Location> close_value_gradient =
                                    gaussian::value_and_gradient(1. / SQR(close_std_dev), vec);

                                pair<double, Location> medium_value_gradient =
                                    gaussian::value_and_gradient(1. / SQR(medium_std_dev), vec);


                                ship_1.close_field[player_ships_2.first] += close_value_gradient.first;
                                ship_1.close_field_gradient[player_ships_2.first] += close_value_gradient.second;

                                ship_1.medium_field[player_ships_2.first] += medium_value_gradient.first;
                                ship_1.medium_field_gradient[player_ships_2.first] += medium_value_gradient.second;

                                ship_1.close_field_health[player_ships_2.first] += close_value_gradient.first * (ship_2.health * (1./255));
                                ship_1.close_field_health_gradient[player_ships_2.first] += close_value_gradient.second * (ship_2.health * (1./255));

                                ship_1.medium_field_health[player_ships_2.first] += medium_value_gradient.first  * (ship_2.health * (1./255));
                                ship_1.medium_field_health_gradient[player_ships_2.first] += medium_value_gradient.second * (ship_2.health * (1./255));
                        }
                    }
                }
            }
        }

        for (int i=0; i<map.planets.size(); i++) {
            Planet & planet = map.planets[i];

            for (pair<const hlt::PlayerId, vector<hlt::Ship>>& player_ships_2 : map.ships) {


                vector<Ship> &ships_2 = player_ships_2.second;
                for(int j=0; j<ships_2.size(); j++) {


                    hlt::Ship &ship_2= ships_2[j];

                    if (ship_2.docking_status == hlt::ShipDockingStatus::Undocked) {


                        Location vec = planet.location - ship_2.location;

                        pair<double, Location> close_value_gradient =
                            gaussian::value_and_gradient(1. / SQR(close_std_dev), vec);

                        pair<double, Location> medium_value_gradient =
                            gaussian::value_and_gradient(1. / SQR(medium_std_dev), vec);


                        planet.close_field[player_ships_2.first] += close_value_gradient.first;
                        planet.close_field_gradient[player_ships_2.first] += close_value_gradient.second;

                        planet.medium_field[player_ships_2.first] += medium_value_gradient.first;
                        planet.medium_field_gradient[player_ships_2.first] += medium_value_gradient.second;

                    }
                }
            }
        }






        if (fleeing) {

            {
                std::ostringstream ss;
                ss << "Fleeing\n";
                hlt::Log::log(ss.str());
            }

            for ( hlt::Ship& ship : map.ships.at(player_id)) {

                if (ship.docking_status != hlt::ShipDockingStatus::Undocked) {
                    continue;
                }


                Location totalGradient = {0.,0.};

                for(int i=0;i< map.ships.size(); i++) {
                    if (i != player_id) {
                        totalGradient += ship.medium_field_gradient[i];
                    }
                }



                if (!(totalGradient == Location({0.,0.}))) {
                    totalGradient *= 1./totalGradient.norm();
                    totalGradient *= -7;
                }

                std::ostringstream ss;
                ss << "Ship " << ship.entity_id << " is " << totalGradient.pos_x << " " <<totalGradient.pos_y << std::endl;
                hlt::Log::log(ss.str());


                 const hlt::Location& target = 
                    ship.location + totalGradient;


                const int max_corrections = 180;
                const bool avoid_obstacles = true;
                const double angular_step_rad = M_PI/ 180.;

                hlt::possibly<hlt::Move> move =
                        hlt::navigation::navigate_ship_towards_target(
                                map, ship, target,  7, avoid_obstacles, max_corrections, angular_step_rad, false);

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
                            

        if (!rushing) {
            for ( hlt::Ship& ship : map.ships.at(player_id)) {
                if (ship.docking_status != hlt::ShipDockingStatus::Undocked) {
                    continue;
                } 

                double min_so_far = 1e20;


                hlt::Planet *target_planet_ptr = nullptr;

                if (!naive_rushing)
                for( int i = 0; i<map.planets.size(); i++) {


                    hlt::Planet &planet = map.planets[i];

                    double docked = planet.docked_ships.size();

                    if (planet.owner_id == player_id || !planet.owned) {
                        docked += planet.targetted * 0.666;
                    }
                    else {
                        docked -= planet.targetted * 0.333;
                    }


                    double distance = ship.location.get_distance_to(planet.location);

                    //distance -= planet.radius;

                    double weight = distance;

                    if (planet.owned && planet.owner_id != player_id) {

                        double cumul_protection = 0;

                        int docked_ships = 0;
                        for(int i=0; i<planet.docked_ships.size(); i++) {
                            Ship &docked_ship = map.get_ship(planet.owner_id, planet.docked_ships[i]);
                            cumul_protection += docked_ship.medium_field[planet.owner_id];
                            if (docked_ship.docking_status == hlt::ShipDockingStatus::Docked) {
                                docked_ships ++;
                            }
                        }

                        double total_protection = cumul_protection / planet.docked_ships.size();

                        weight += (docked*4./7. + weight*docked*(1./12.)*4) * total_protection * 0.03;

                        /*
                           if(weight/7. < 12./docked) {
                           weight *= 0.5;
                           }

                           weight = weight*0.8 +  //arbitrary number
                           docked*4./7. + weight*docked*(1./12.)*4;
                         */


                    }
                    else {

                        double threat = 0;

                        for(int i=0;i< map.ships.size(); i++) {
                            if (i != player_id) {
                                threat += planet.medium_field[i];
                            }
                        }

                        //threat -= planet.medium_field[player_id];


                        //threat /= map.ships[player_id].size();

                        weight *= 1 + ((double)docked/planet.docking_spots);

                        /*
                           if (!planet.targetted && planet.docked_ships.size() == 0 && threat > 1) {
                        //weight *= (1 + threat * 0.5);
                        continue;
                        }
                         */

                    }

                    if (planet.owner_id != player_id || !planet.is_full()){
                        if (weight < min_so_far) {
                            target_planet_ptr = &map.planets[i];
                            min_so_far = weight;
                        }
                    }
                }

                auto it = map.ships.begin();



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

                            if (naive_rushing || (enemy.docking_status == hlt::ShipDockingStatus::Undocked
                                    || enemy.docking_status == hlt::ShipDockingStatus::Undocking)){

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

                    bool go_straight = true;

                    double dist = ship_ptr -> location.get_distance_to ( ship.location );

                    if ( dist < 12 ) {

                        /*
                           Location close_enemy_gradient = {0.,0.};
                           Location medium_enemy_gradient = {0.,0.};

                           double close_field = 0;
                           double medium_field = 0;

                           for(int i=0;i< map.ships.size(); i++) {
                           if (i != player_id) {
                           close_field += ship.close_field[i];
                           medium_field += ship.medium_field[i];
                           close_enemy_gradient += ship.close_field_gradient[i];
                           medium_enemy_gradient += ship.medium_field_gradient[i];
                           }
                           }
                         */

                        if (
                                //ship_ptr->close_field_health[ship_ptr->owner_id]
                                ship_ptr->close_field[ship_ptr->owner_id]
                                >
                                //ship.close_field_health[player_id]
                                ship.close_field[player_id]
                           ) {

                            go_straight = false;

                            Location gradient = 
                                ship.medium_field_gradient[player_id]
                                //+ ship.close_field_health_gradient[player_id]
                                //+ (ship.close_field_health_gradient[ship_ptr->owner_id]
                                //+ ship.close_field_gradient[ship_ptr->owner_id] * -0.75 ) * 2; 
                                ;


                            /*
                               if (ship.health <= 64 ) {
                               gradient += ship.close_field_gradient[ship_ptr->owner_id] * -1.;
                               }
                             */

                            Location diff = ship.location - ship_ptr->location;

                            if (!(diff == Location({0.,0.}))) {
                                diff *= 1./diff.norm();
                                diff *= 7.;
                            }


                            if (!(gradient == Location({0.,0.}))) {
                                gradient *= 1./gradient.norm();
                                gradient *= 7.;
                            }
                            else {
                                gradient = diff * -1.;
                            }


                            const hlt::Location& target = 
                                ship.location + gradient;


                            const int max_corrections = 180;
                            const bool avoid_obstacles = true;
                            const double angular_step_rad = M_PI/ 180.;

                            hlt::possibly<hlt::Move> move =
                                hlt::navigation::navigate_ship_towards_target(
                                        map, ship, target,  7, avoid_obstacles, max_corrections, angular_step_rad, false);

                            if (move.second) {
                                moves.push_back(move.first);
                                double angle_rad  = move.first.move_angle_deg * (M_PI * 2. /360.);
                                double thrust = move.first.move_thrust;
                                ship.thrust.pos_x = cos(angle_rad) * thrust;
                                ship.thrust.pos_y = sin(angle_rad) * thrust;
                                continue;
                            }
                        }
                    }

                    if ( go_straight) {

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

                }



                if (target_planet_ptr == nullptr) continue;
                hlt::Planet &planet = *target_planet_ptr;

                if (is_planet_in_center [planet.entity_id] == true) {
                    number_going_to_center ++;
                }


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

            if (time == 1 &&  should_rush(map)) {


                moves.clear();

                // Put ships in special formation

                vector<pair<double, EntityId> > v;

                for(int i=0; i<3; i++) {
                    v.emplace_back(map.ships[player_id][i].location.pos_y, map.ships[player_id][i].entity_id);
                }
                sort(v.begin(), v.end());

                rushing = true;

                rusher.location = map.get_ship(player_id, v[1].second).location;
                rusher.radius = 1.2;
                rusher.entity_id = -1;

                if (map.ships[player_id][1].location.pos_y  < map.map_height * 0.333) {

                    if (map.ships[player_id][1].location.pos_x < map.map_width * 0.333) {

                        moves.push_back( Move::thrust(v[0].second, 6, 180 - 113) );
                        moves.push_back( Move::thrust(v[1].second, 3, 180 - 116) );
                        moves.push_back( Move::thrust(v[2].second, 2, 180 - 166) );
                        rusher.location += Location({ 1.6887, 2.90109});

                    }
                    else {

                        moves.push_back( Move::thrust(v[0].second, 6, 113) );
                        moves.push_back( Move::thrust(v[1].second, 3, 116) );
                        moves.push_back( Move::thrust(v[2].second, 2, 166) );
                        rusher.location += Location({-1.6887, 2.90109});

                    }
                }
                else {
                    if (map.ships[player_id][1].location.pos_x < map.map_width * 0.333) {

                        moves.push_back( Move::thrust(v[2].second, 6, 180 + 113) );
                        moves.push_back( Move::thrust(v[1].second, 3, 180 + 116) );
                        moves.push_back( Move::thrust(v[0].second, 2, 180 + 166) );
                        rusher.location += Location({1.6887, -2.90109});

                    }
                    else {

                        moves.push_back( Move::thrust(v[2].second, 6, 360 - 113) );
                        moves.push_back( Move::thrust(v[1].second, 3, 360 - 116) );
                        moves.push_back( Move::thrust(v[0].second, 2, 360 - 166) );
                        rusher.location += Location({-1.6887, -2.90109});

                    }
                }

            }

            if (!hlt::out::send_moves(moves)) {
                hlt::Log::log("send_moves failed; exiting");
                break;
            }

            continue;
        }
        // Else rushing
        else {


            double min_so_far = 1e20;
            hlt::Ship *ship_ptr = nullptr;

            auto it = map.ships.begin();


            for(; it != map.ships.end(); it++) {

                if(it->first != player_id) {

                    std::vector<hlt::Ship> &v = it->second;

                    for(int i=0; i<v.size(); i++) {

                        hlt::Ship &enemy = v[i];

                        double distance = rusher.location.get_distance_to(enemy.location);
                        double weight = distance;
                        if (enemy.docking_status != hlt::ShipDockingStatus::Undocked) {
                            Planet &planet = map.get_planet( enemy.docked_planet );
                            for(int i=0; i<planet.docked_ships.size(); i++) {
                                weight *= 0.5;
                            }
                        }

                        if (weight < min_so_far) {
                            min_so_far = weight;
                            ship_ptr = &v[i];
                        }
                    }
                }
            }


            assert( ship_ptr != nullptr);

            hlt::Ship &enemy = *ship_ptr;



            auto wew = map.ships[player_id];
            map.ships[player_id].clear();

            const hlt::possibly<hlt::Move> move =
                hlt::navigation::navigate_ship_to_dock(map, rusher, enemy, hlt::constants::MAX_SPEED);

            if (move.second) {
                for(size_t i=0; i<wew.size(); i++) {
                    hlt::Move m = move.first;
                    m.ship_id = wew[i].entity_id;
                    moves.push_back(m);
                }

                double angle_rad  = move.first.move_angle_deg * (M_PI * 2. /360.);
                double thrust = move.first.move_thrust;
                rusher.location.pos_x += cos(angle_rad) * thrust;
                rusher.location.pos_y += sin(angle_rad) * thrust;

                //ship_ptr -> targetted ++;
            }

            if (!hlt::out::send_moves(moves)) {
                hlt::Log::log("send_moves failed; exiting");
                break;
            }

            continue;

        }
    }
}
