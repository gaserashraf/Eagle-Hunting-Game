#include "player.hpp"
#include "../deserialize-utils.hpp"


namespace our{

    void Player::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return;
        health = data.value("health", health);
        radius = data.value("radius", radius);
    }
}