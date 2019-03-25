#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Program {
public:
    Program(GLFWwindow* window) {
        this->window = window;
    }

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
        while (!glfwWindowShouldClose(window)) {
            // Clear colour and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            update();
            render();

            // Handle other events
            glfwPollEvents();
            handleKeyboardInput();

            // Flip buffers
            glfwSwapBuffers(window);
        }
    }

private:
    void handleKeyboardInput() {
        // Use escape key for terminating the GLFW window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    GLFWwindow* window;
};
