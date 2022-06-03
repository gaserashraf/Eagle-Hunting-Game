#include "skyLight.hpp"
#include "../deserialize-utils.hpp"


namespace our{

    void skyLight::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return;
        topLight    = data.value("topLight", topLight);
        middleLight = data.value("middleLight", middleLight);
        bottomLight = data.value("bottomLight", bottomLight);
    }
}