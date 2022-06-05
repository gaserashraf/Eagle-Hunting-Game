#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2


namespace our{

    class Light : public Component{
        public:
            int lightType = DIRECTIONAL;    
            glm::vec3 diffuse=glm::vec3(0.0,0.0,0.0);
            glm::vec3 specular = glm::vec3(0.0, 0.0, 0.0);
            glm::vec3 attenuation = glm::vec3(1.0,0.0,0.0);
            glm::vec2 cone_angles = glm::vec2(45, 90);

            static std::string getID() { return "Light"; }
          
            void deserialize(const nlohmann::json &data) override;
    };
}