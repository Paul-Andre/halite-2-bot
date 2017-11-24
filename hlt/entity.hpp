#pragma once

#include "location.hpp"
#include "types.hpp"

namespace hlt {
    struct Entity {
        EntityId entity_id;
        PlayerId owner_id;
        Location location;
        int health;
        double radius;

        Location thrust = {0,0};

        bool is_alive() const {
            return health > 0;
        }
    };
}
