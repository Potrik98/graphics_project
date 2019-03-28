#pragma once

#include <glad/glad.h>
#include "core/program.h"

class Demo : public Program {
public:
    Demo(GLFWwindow* window) : Program(window) {
        windowParameters.windowWidth = 1920;
        windowParameters.windowHeight = 1080;
        windowParameters.windowSamples = 4;
        windowParameters.windowResizable = GL_TRUE;
        windowParameters.windowTitle = "Graphics project demo";
    }

    void update() override;

    void render() override;

    void init() override;
};
