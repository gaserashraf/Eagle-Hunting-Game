#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/duck-controller.hpp>
#include <systems/scope-controller.hpp>
#include <systems/collision-controller.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State
{

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::DuckController duckController;
    our::ScopeController scopeController;
    our::CollisionController collisionController;
    bool startGame = false;
    bool gameover = false;
    void onInitialize() override
    {
        if (startGame)
            return;
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world"))
        {
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        startGame = true;
    }

    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        if(!gameover){
            movementSystem.update(&world, (float)deltaTime);
            cameraController.update(&world, (float)deltaTime);
            duckController.update(&world, (float)deltaTime);
            scopeController.update(&world, (float)deltaTime);
            gameover = collisionController.update(&world, (float)deltaTime);
            renderer.render(&world);
        }
    }

    void onDestroy() override
    {
      //  return;
        // // Don't forget to destroy the renderer
        // renderer.destroy();
        // // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        // cameraController.exit();
        // // and we delete all the loaded assets to free memory on the RAM and the VRAM
        // our::clearAllAssets();
    }
};