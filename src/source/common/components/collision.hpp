#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>



namespace our{

    class Collision : public Component{
        public:
            int effect = 20;
            int radius = 1;
            static std::string getID() { return "Collision"; }
          
            void deserialize(const nlohmann::json &data) override;
    };
}