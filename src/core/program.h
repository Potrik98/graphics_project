#pragma once

#include "window.h"
#include <string>

class Program {
public:
    explicit Program(const WindowParameters& windowParameters) : window(windowParameters) {}

    virtual void update() = 0;

    virtual void render() = 0;

    virtual void init() = 0;

    void run() {
        // Enable depth (Z) buffer (accept "closest" fragment)
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Configure miscellaneous OpenGL settings
        glEnable(GL_CULL_FACE);

        // Set default colour after clearing the colour buffer
        glClearColor(0.3f, 0.5f, 0.8f, 1.0f);

        init();

        // Rendering Loop
        while (!glfwWindowShouldClose(window.glfwWindow)) {
            // Clear colour and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            update();
            render();

            // Handle other events
            glfwPollEvents();
            handleKeyboardInput();

            // Flip buffers
            glfwSwapBuffers(window.glfwWindow);
        }
    }

private:
    void handleKeyboardInput() {
        // Use escape key for terminating the GLFW window
        if (glfwGetKey(window.glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.glfwWindow, GL_TRUE);
        }
    }

    Window window;
};
