#pragma once

#include "../ecs/world.hpp"
#include "../components/duck.hpp"
#include "../components/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>
namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class DuckController
    {
    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) {
            std::vector<Entity*> ducks;
            Entity* camera;

            // For each entity in the world
            for(auto entity : world->getEntities()){
                // Get the movement component if it exists
                Duck* d= entity->getComponent<Duck>();
                // If the movement component exists
                if(d){
                    ducks.push_back(entity);
                    //std::cout << "Duck found" << std::endl;
                }
                CameraComponent* c= entity->getComponent<CameraComponent>();
                if(c){
                    camera=entity;
                  //  std::cout << "Camera found" << std::endl;
                }
            }
            auto cameraPos=camera->localTransform.position;
            for(auto duck :ducks){
                auto duckPos=duck->localTransform.position;
                auto dir=glm::normalize(cameraPos-duckPos);
                Duck *d=duck->getComponent<Duck>();

                duck->localTransform.position+=deltaTime*dir*(float)d->speed;
                duck->localTransform.rotation= glm::vec3(glm::rotate(glm::mat4(1.0f),glm::atan(dir.z,dir.x),glm::vec3(0,-1,0))*glm::vec4(0,0,-1,0));
            }
        }

    };

}
