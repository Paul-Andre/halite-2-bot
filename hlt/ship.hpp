#pragma once

#include "constants.hpp"
#include "types.hpp"
#include "planet.hpp"

namespace hlt {
    /// The states a ship can be in regarding docking.
    enum class ShipDockingStatus {
        Undocked = 0,
        Docking = 1,
        Docked = 2,
        Undocking = 3,
    };

    struct Ship : Entity {
        /// The turns left before the ship can fire again.
        int weapon_cooldown;

        int targetted = 0;

        int hitting = 0;

        double distance_to_my_closest_planet = 10000;


        // Create a gaussian forcefield around undocked enemy ships
        double close_field[4] = {0};
        Location close_field_gradient[4] = {};

        double medium_field[4] = {0};
        Location medium_field_gradient[4] = {};

        double close_field_health[4] = {0};
        Location close_field_health_gradient[4] = {};

        double medium_field_health[4] = {0};
        Location medium_field_health_gradient[4] = {};


        double threat;

        // What planets the enimy ship might be planning to attack
        //vector<EntityId> planning_to_attack;

        // How valuable this ship is to us.
        //double value;

        ShipDockingStatus docking_status;

        /// The number of turns left to complete (un)docking.
        int docking_progress;

        /// The id of the planet this ship is docked to. Only valid if
        /// Ship::docking_status is -not- DockingStatus::Undocked.
        EntityId docked_planet;

        /// Check if this ship is close enough to dock to the given planet.
        bool can_dock(const Planet& planet) const {
            return location.get_distance_to(planet.location) <= (constants::SHIP_RADIUS + constants::DOCK_RADIUS + planet.radius);
        }
    };
}
