#pragma once

#include <application.hpp>
#include <imgui-utils/utils.hpp>
#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/duck-controller.hpp>
#include <asset-loader.hpp>
#include <string>
// This state shows how to use the ECS framework and deserialization.
class GameMenustate : public our::State
{
    bool startGame = true;
    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
    }

    void onImmediateGui() override
    {

        ImGui::Begin("Welcome to Duck hunting game!");        // title
        ImGui::SetWindowSize(ImVec2((float)600, (float)400)); // size of the imgui
        ImGui::SetWindowFontScale(3.0f);                      // make the font big

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(247,107,32,255));
        ImGui::Text("Game Menu");                             // first text
        ImGui::PopStyleColor();
        ImGui::SetWindowFontScale(2.0f); 
        ImGui::Text("     ");        
        if (startGame)
        {
            if (ImGui::Button("New Game"))
            {
                startGame = false;
                getApp()->changeState("main");
            }
        }
        else
        {
            if (ImGui::Button("Continue"))
            {
                getApp()->changeState("main");
            }
        }
        // exit button
        if (ImGui::Button("Exit"))
        {
            glfwSetWindowShouldClose(getApp()->getWindow(), GLFW_TRUE);
        }
        ImGui::End();
    }
    void onDraw(double deltaTime) override
    {
        glClearColor(0.0, 0.0, 0., 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void onDestroy() override
    {

    }
};