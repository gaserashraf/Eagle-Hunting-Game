#include "collision.hpp"
#include "../deserialize-utils.hpp"


namespace our{

    void Collision::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return;
        effect = data.value("effect", effect);
        radius = data.value("radius", radius);
    }
}