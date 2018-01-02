#pragma once

#include "map.hpp"
#include "types.hpp"
#include "ship.hpp"
#include "planet.hpp"

namespace hlt {
    class Map {
    public:
        int map_width, map_height;

        std::vector< std::vector<Ship> > ships;
        std::vector< entity_map<unsigned int>> ship_map;

        std::vector<Planet> planets;
        entity_map<unsigned int> planet_map;

        Map(int width, int height);

        Ship& get_ship(const PlayerId player_id, const EntityId ship_id)  {
            return ships.at(player_id).at(ship_map.at(player_id).at(ship_id));
        }

        Planet& get_planet(const EntityId planet_id)  {
            return planets.at(planet_map.at(planet_id));
        }
    };
}
