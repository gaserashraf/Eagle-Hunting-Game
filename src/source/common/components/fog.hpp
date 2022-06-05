#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>




namespace our{

    class Fog : public Component{
        public:
            glm::vec3 fog_color = glm::vec3(0.86274,0.86274,0.86274);
            float fog_power = 45.0f;
            float fog_distance = 1.5f;

            static std::string getID() { return "Fog"; }

            void deserialize(const nlohmann::json &data) override;
    };
}