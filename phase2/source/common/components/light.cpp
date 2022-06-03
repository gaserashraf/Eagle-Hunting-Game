#include "light.hpp"
#include "../deserialize-utils.hpp"


namespace our{

    void Light::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return;
        std::string type = data.value("lightType", "directional");
        if (type=="directional"){
            lightType= DIRECTIONAL;  
        }
        else if(type=="point"){
            lightType= POINT;
            attenuation = data.value("attenuation", attenuation);
        }else if (type=="spot"){
            lightType= SPOT;
            cone_angles = data.value("cone_angles", cone_angles);
        }
        diffuse = data.value("diffuse", diffuse);
        specular = data.value("specular", specular);
    }
}