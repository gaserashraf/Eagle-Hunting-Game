#include "duck.hpp"
#include "../deserialize-utils.hpp"


namespace our{

    void Duck::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return;

        speed = data.value("speed", speed);
    }
}