#pragma once

#include "collision.hpp"
#include "map.hpp"
#include "move.hpp"
#include "util.hpp"

namespace hlt {
    namespace navigation {
        static bool check_and_add_entity_between(
                const Location& start,
                const Location& target,
                const Entity& my_entity,
                const Entity& entity_to_check)
        {
            //double tot_radius = my_entity.radius + entity_to_check.radius + constants::FORECAST_FUDGE_FACTOR;

            const Location &location = entity_to_check.location;
            if (location == start || location == target) {
                return false;
            }

            if (entity_to_check.thrust == Location({0,0})) {
                if (collision::segment_circle_intersect(start, target, entity_to_check, constants::FORECAST_FUDGE_FACTOR)) {
                    return true;
                }
            }

            return false;

        }


        static bool check_moving_collision(Location x, Location y, Location dx, Location dy, double tot_radius) {

            int steps = 20;

            dx *= (1./steps);
            dy *= (1./steps);

            for(int i=0; i<=steps; i++) {
                if (x.get_distance_to(y) <= tot_radius) {
                    return true;
                }
                x += dx;
                y += dy;
            }

            return false;
        }



        static bool objects_between(const Map& map, const Location& start, const Location& target,
                const Entity& my_entity) {

            for (const Planet& planet : map.planets) {
                if (check_and_add_entity_between( start, target, my_entity, planet)) return true;
            }

            for (const auto& player_ship : map.ships) {
                for (const Ship& ship : player_ship.second) {
                    if (check_and_add_entity_between( start, target, my_entity, ship)) return true;
                }
            }

            double dist = target.get_distance_to(start);
            double thrust = std::min(7, (int)dist);

            double angle_rad = start.orient_towards_in_rad(target);

            const int angle_deg = util::angle_rad_to_deg_clipped(angle_rad);
            angle_rad = angle_deg * (2. * M_PI / 360.);


            Location dx = {
                cos(angle_rad) * thrust,
                sin(angle_rad) * thrust
            };


            for (const auto& player_ship : map.ships) {
                for (const Ship& ship : player_ship.second) {
                    if (!(ship.location == my_entity.location)) {
                        const double tot_radius = my_entity.radius + ship.radius;
                        if (check_moving_collision(my_entity.location, ship.location, dx, ship.thrust, tot_radius)) return true;
                    }
                }
            }

            return false;
        }

        static possibly<Move> navigate_ship_towards_target(
                const Map& map,
                const Ship& ship,
                const Location& target,
                const int max_thrust,
                const bool avoid_obstacles,
                const int max_corrections,
                const double angular_step_rad)
        {
            if (max_corrections <= 0) {
                return { Move::noop(), false };
            }

            const double distance = ship.location.get_distance_to(target);
            const double angle_rad = ship.location.orient_towards_in_rad(target);

            if (avoid_obstacles && objects_between(map, ship.location, target, ship)) {
                const double new_target_dx = cos(angle_rad + angular_step_rad) * distance;
                const double new_target_dy = sin(angle_rad + angular_step_rad) * distance;
                const Location new_target = { ship.location.pos_x + new_target_dx, ship.location.pos_y + new_target_dy };

                return navigate_ship_towards_target(
                        map, ship, new_target, max_thrust, true, (max_corrections - 1), angular_step_rad);
            }

            int thrust;
            if (distance < max_thrust) {
                // Do not round up, since overshooting might cause collision.
                thrust = (int) distance;
            } else {
                thrust = max_thrust;
            }

            const int angle_deg = util::angle_rad_to_deg_clipped(angle_rad);

            return { Move::thrust(ship.entity_id, thrust, angle_deg), true };
        }

        static possibly<Move> navigate_ship_to_dock(
                const Map& map,
                const Ship& ship,
                const Entity& dock_target,
                const int max_thrust)
        {
            const int max_corrections = constants::MAX_NAVIGATION_CORRECTIONS;
            const bool avoid_obstacles = true;
            const double angular_step_rad = M_PI / 180.0;
            const Location& target = ship.location.get_closest_point(dock_target.location, dock_target.radius);

            return navigate_ship_towards_target(
                    map, ship, target, max_thrust, avoid_obstacles, max_corrections, angular_step_rad);
        }
    }
}
