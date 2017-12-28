#pragma once

#include <vector>

#include "types.hpp"
#include "entity.hpp"

namespace hlt {
    struct Planet : Entity {
        bool owned;

        /// The remaining resources.
        int remaining_production;

        /// The currently expended resources.
        int current_production;

        /// The maximum number of ships that may be docked.
        unsigned int docking_spots;

        /// Contains IDs of all ships in the process of docking or undocking,
        /// as well as docked ships.
        std::vector<EntityId> docked_ships;

        // how much of my ships are planning to go to that planet
        int targetted = 0;

        // how much incoming enemy ships my planet will send ships against
        int planning_to_defend = 0;

        // Create a gaussian forcefield around undocked enemy ships
        double close_field[4];
        Location close_field_gradient[4];

        double medium_field[4];
        Location medium_field_gradient[4];


        bool is_full() const {
            return docked_ships.size() == docking_spots;
        }
    };
}
