#pragma once

#include "../ecs/world.hpp"
#include "../components/player.hpp"
#include "../components/collision.hpp"

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
    class CollisionController
    {
    public:
        int health = 100;
        // This should be called every frame to update all entities containing a MovementComponent.
        bool update(World *world, float deltaTime)
        {
            std::vector<Entity *> collisions;
            Entity *player;

            for (auto entity : world->getEntities())
            {
                Player *p = entity->getComponent<Player>();
                if (p)
                {
                    player = entity;
                }
                Collision *c = entity->getComponent<Collision>();
                if (c)
                {
                    collisions.push_back(entity);
                }
            }
            Player *playerComponent = player->getComponent<Player>();

            glm::vec3 playerPos = player->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);

            for (int i = 0; i < collisions.size(); i++)
            {
                Collision *collisionComponent = collisions[i]->getComponent<Collision>();
                glm::vec3 collisionPos = collisions[i]->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                if (glm::distance(playerPos, collisionPos) <= playerComponent->radius + collisionComponent->radius)
                {
                    playerComponent->health -= collisionComponent->effect;
                    std::cout << "player health: " << playerComponent->health << std::endl;
                    health = playerComponent->health;
                    collisions[i]->getWorld()->markForRemoval(collisions[i]);
                    player->getWorld()->deleteMarkedEntities();
                    if (playerComponent->health <= 0)
                    {
                        std::cout << "You died" << std::endl;
                        return true;
                    }
                }
            }
            return false;
        }
    };

}
