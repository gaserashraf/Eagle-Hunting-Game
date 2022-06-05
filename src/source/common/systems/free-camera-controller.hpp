#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/scope.hpp"
#include "../components/duck.hpp"
#include "../components/free-camera-controller.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem
    {
        Application *app;          // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked

        const int winCnt = 10;

    public:
        int cntDucks = 0;
        
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        bool update(World *world, float deltaTime)
        {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent *camera = nullptr;
            FreeCameraControllerComponent *controller = nullptr;
            for (auto entity : world->getEntities())
            {
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if (camera && controller)
                    break;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if (!(camera && controller))
                return 0;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity *entity = camera->getOwner();

            // If the left mouse button is pressed, we lock and hide the mouse. This common in First Person Games.
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && !mouse_locked)
            {
                app->getMouse().lockMouse(app->getWindow());
                mouse_locked = true;
                // If the left mouse button is released, we unlock and unhide the mouse.
            }
            else if (!app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && mouse_locked)
            {
                app->getMouse().unlockMouse(app->getWindow());
                mouse_locked = false;
            }

            // We get a reference to the entity's position and rotation
            glm::vec3 &position = entity->localTransform.position;
            glm::vec3 &rotation = entity->localTransform.rotation;

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1))
            {
                glm::vec2 delta = app->getMouse().getMouseDelta();
                rotation.x -= delta.y * controller->rotationSensitivity; // The y-axis controls the pitch
                rotation.y -= delta.x * controller->rotationSensitivity; // The x-axis controls the yaw
            }

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if (rotation.x < -glm::half_pi<float>() * 0.99f)
                rotation.x = -glm::half_pi<float>() * 0.99f;
            if (rotation.x > glm::half_pi<float>() * 0.99f)
                rotation.x = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time.
            rotation.y = glm::wrapAngle(rotation.y);

            // We update the camera fov based on the mouse wheel scrolling amount
            float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
            camera->fovY = fov;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)),
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
                current_sensitivity *= controller->speedupFactor;

            // We change the camera position based on the keys WASD/QE
            // S & W moves the player back and forth
            if (app->getKeyboard().isPressed(GLFW_KEY_W))
                position += front * (deltaTime * current_sensitivity.z);
            if (app->getKeyboard().isPressed(GLFW_KEY_S))
                position -= front * (deltaTime * current_sensitivity.z);
            // A & D moves the player left or right
            if (app->getKeyboard().isPressed(GLFW_KEY_D))
                position += right * (deltaTime * current_sensitivity.x);
            if (app->getKeyboard().isPressed(GLFW_KEY_A))
                position -= right * (deltaTime * current_sensitivity.x);

            if (app->getKeyboard().isPressed(GLFW_KEY_ESCAPE))
            {
                app->changeState("game-menu-state");
                return 0;
            }

            position.y = 0;
            if (position.x > 8)
                position.x = 8;
            if (position.z > 8)
                position.z = 8;
            if (position.x < -8)
                position.x = -8;
            if (position.z < -8)
                position.z = -8;

            // player attack ducks

            auto cameraPos = camera->getOwner()->localTransform.position;
            Entity *scope;
            // For each entity in the world
            std::vector<Entity *> ducks;
            for (auto entity : world->getEntities())
            {
                Scope *s = entity->getComponent<Scope>();
                if (s)
                {
                    scope = entity;
                }
                Duck *d = entity->getComponent<Duck>();
                // If the movement component exists
                if (d)
                {
                    ducks.push_back(entity);
                    // std::cout << "Duck found" << std::endl;
                }
            }
            glm::mat4 matrixScope = scope->getLocalToWorldMatrix();
            glm::vec3 frontScope = glm::vec3(matrixScope * glm::vec4(0, 0, -1, 0)),
                      postionScope = glm::vec3(matrixScope * glm::vec4(0, 0, 0, 1));
            // glm::vec3 postionScope = scope->localTransform.position;
            if (app->getKeyboard().isPressed(GLFW_KEY_X))
            {
                /*
                scope info from game.jsonc:
                "position": [0, 0.2, -2],
                "rotation": [90, 0, 0],
                "scale": [2.9, 2.9, 2.9],
                */

                // check for every duck if this duck on the line
                for (auto duck : ducks)
                {

                    glm::mat4 matrixDuck = duck->getLocalToWorldMatrix();
                    glm::vec3 positionDuck = glm::vec3(matrixDuck * glm::vec4(0, 0, 0, 1));

                    // first get the equ of the line
                    /*
                    x0:(0,0,0), x1=(positionDuck)
                    x(t) = x0 + t(x1 - x0)
                    y(t) = y0 + t(y1 - y0)
                    z(t) = z0 + t(z1 - z0)
                    t = (x - x0)/(x1-x0)

                    if ( (y0 + t(y1-y0) == y) and (z0 + t(z1-z0) == z) ) then
                    ---> we are in the line
                    */
                    float x0 = postionScope.x;
                    float y0 = postionScope.y;
                    float z0 = postionScope.z;

                    float x1 = frontScope.x;
                    float y1 = frontScope.y;
                    float z1 = frontScope.z;

                    float x = positionDuck.x;
                    float y = positionDuck.y;
                    float z = positionDuck.z;
                    glm::vec3 t = positionDuck - postionScope;
                    float t1 = t.x / (x1);
                    float t2 = t.y / (y1);
                    float t3 = t.z / (z1);
                    const float EPS = 2;
                    std::cout << t1 << " " << t2 << " " << t3 << "\n";
                    if (abs(t1 - t3) < 0.5 && abs(t1 - t2) < 0.5 && abs(t2 - t3) < 0.5)
                    {

                        cntDucks++;
                        duck->getWorld()->markForRemoval(duck);
                        // std::cout << "hit duck " << deltaTime * 100.0 << "\n";
                    }
                    scope->getWorld()->deleteMarkedEntities();
                    if (cntDucks >= winCnt)
                        return 1;
                }
            }
            return 0;
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit()
        {
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };

}
