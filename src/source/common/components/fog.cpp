#include "fog.hpp"
#include "../deserialize-utils.hpp"


namespace our{

    void Fog::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return;
        fog_color = data.value("fog_color", fog_color);
        fog_power = data.value("fog_power", fog_power);
        fog_distance = data.value("fog_distance", fog_distance);   
    }
}