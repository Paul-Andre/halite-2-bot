#include "hlt/hlt.hpp"
#include <cassert>
#include "hlt/navigation.hpp"

int main() {

    const hlt::Metadata metadata = hlt::initialize("EvenMoreTerrible");
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




    for (;;) {

        std::vector<hlt::Move> moves;

        hlt::Map map = hlt::in::get_map();

        /*
        vector<hlt::Ship*> enemy_ships;
        for(auto p: map.ships) {
            if (p.first != player_id) {
                for (int i=0; i< p.second.size(); i++) {
                    enemy_ships.push_back(&p.second[i]);
                }
            }
        }
        */

        for (const hlt::Ship& ship : map.ships.at(player_id)) {
            if (ship.docking_status != hlt::ShipDockingStatus::Undocked) {
                continue;
            }

            double min_so_far = 1e20;


            if (map.planets.size() == 0) break; // todo ... wew;
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

            if (target_planet_ptr == nullptr) break;
            hlt::Planet &planet = *target_planet_ptr;

            planet.targetted ++;
            //std::cerr << planet.targetted;

            /*
            std::ostringstream out;
            out << planet.targetted <<" adfas adfa adsfa asdf adfa "<<std::endl;
            hlt::Log::log(out.str());
            */

            if (planet.owner_id == player_id || !planet.owned) {

                if (ship.can_dock(planet)) {
                    moves.push_back(hlt::Move::dock(ship.entity_id, planet.entity_id));
                }
                else {
                    const hlt::possibly<hlt::Move> move =
                        hlt::navigation::navigate_ship_to_dock(map, ship, planet, hlt::constants::MAX_SPEED);
                    if (move.second) {
                        moves.push_back(move.first);
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
                }
            }

        }

        if (!hlt::out::send_moves(moves)) {
            hlt::Log::log("send_moves failed; exiting");
            break;
        }

    }
}
