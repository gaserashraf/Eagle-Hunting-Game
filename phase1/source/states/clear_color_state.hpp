#pragma once

#include <application.hpp>

class ClearColorState : public our::State
{
    // onInitialize() function is called once before the state starts
    void onInitialize() override
    {
        // TODO: Read the color from the configuration file and use it to set the clear color for the window
        // HINT: you can the configuration for the whole application using "getApp()->getConfig()"
        // To see how the clear color is written in the json files, see "config/blue-screen.json"
        // To know how read data from a nlohmann::json object,
        // look at the following documentation: https://json.nlohmann.me/features/element_access/
        nlohmann::json app_config = getApp()->getConfig();
        const float clearColor[] = {
            app_config["scene"]["clear-color"]["r"],
            app_config["scene"]["clear-color"]["g"],
            app_config["scene"]["clear-color"]["b"]};
        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0);
        std::cout << "colors : ";
        std::cout << clearColor[0] << " " << clearColor[1] << " " << clearColor[2] << std::endl;
    }

    // onDraw(deltaTime) function is called every frame
    void onDraw(double deltaTime) override
    {
        // At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.
        glClear(GL_COLOR_BUFFER_BIT);
    }
};