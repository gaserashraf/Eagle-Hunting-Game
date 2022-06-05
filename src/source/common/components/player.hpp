#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>



namespace our{

    class Player : public Component{
        public:
            int health = 100;
            int radius = 1;
            static std::string getID() { return "Player"; }
          
            void deserialize(const nlohmann::json &data) override;
    };
}