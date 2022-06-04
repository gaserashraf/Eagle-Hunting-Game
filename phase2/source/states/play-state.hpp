#pragma once

#include <application.hpp>
#include "../common/ecs/world.hpp"
#include "../common/ecs/entity.hpp"
//#include <ecs/world.hpp>
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
    bool winGame = false;
    our::Player *player;
    our::Entity *Eplayer;
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

        // For each entity in the world
        // Entity *pplayer;
        /*for (auto entity : world->getEntities())
        {
            Player *p = entity->getComponent<Player>();
            if (p)
            {
                player = entity;
            }
        }*/
        /* Player *playerComponent = player->getComponent<Player>();
         player = playerComponent;*/
        startGame = true;
    }
    void onImmediateGui() override
    {

        ImGui::Begin("Info");                                // title
        ImGui::SetWindowSize(ImVec2((float)100, (float)80)); // size of the imgui
                                                             // ImGui::SetWindowFontScale(3.0f);                      // make the font big

        int cnt = cameraController.cntDucks;
        int health = collisionController.health;
        std::string tmp = "score : " + std::to_string(cnt * 10);
        char *char_arr;

        char_arr = &tmp[0];
        ImGui::Text(char_arr);

        tmp = "health : " + std::to_string(health);
        char *char_arr1;

        char_arr1 = &tmp[0];
        ImGui::Text(char_arr1);
        ImGui::End();
    }
    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        if (!gameover)
        {
            movementSystem.update(&world, (float)deltaTime);
            winGame |= cameraController.update(&world, (float)deltaTime);
            duckController.update(&world, (float)deltaTime);
            scopeController.update(&world, (float)deltaTime);
            gameover = collisionController.update(&world, (float)deltaTime);
            renderer.render(&world);
        }
        else
        {

            getApp()->changeState("game-over-state");
        }
        if (winGame)
        {
            getApp()->changeState("win-state");
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