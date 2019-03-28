#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>

struct WindowParameters {
    unsigned int windowWidth;
    unsigned int windowHeight;
    unsigned int windowSamples;
    GLint windowResizable;
    std::string windowTitle;
};

class Window {
    public:
    Window(WindowParameters windowParameters) {

    }
};
