#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>




namespace our{

    class skyLight : public Component{
        public:
           
            glm::vec3 topLight    = glm::vec3(0.0, 0.0, 0.0);
            glm::vec3 middleLight = glm::vec3(0.0, 0.0, 0.0);
            glm::vec3 bottomLight = glm::vec3(0.0, 0.0, 0.0);
         
            static std::string getID() { return "skyLight"; }
            
            void deserialize(const nlohmann::json &data) override;
    };
}