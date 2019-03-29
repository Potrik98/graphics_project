#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <cstdio>

struct WindowParameters {
    unsigned int windowWidth;
    unsigned int windowHeight;
    unsigned int windowSamples;
    GLint windowResizable;
    std::string windowTitle;
};

// A callback which allows GLFW to report errors whenever they occur
static void glfwErrorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW returned an error:\n\t%s (%i)\n", description, error);
}

class Window {
public:
    Window(const WindowParameters& windowParameters) {
        // Initialise GLFW
        if (!glfwInit()) {
            fprintf(stderr, "Could not start GLFW\n");
            exit(EXIT_FAILURE);
        }

        // Set core window options (adjust version numbers if needed)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Enable the GLFW runtime error callback function defined previously.
        glfwSetErrorCallback(glfwErrorCallback);

        // Set additional window options
        glfwWindowHint(GLFW_RESIZABLE, windowParameters.windowResizable);
        glfwWindowHint(GLFW_SAMPLES, windowParameters.windowSamples);  // MSAA

        // Create window using GLFW
        glfwWindow = glfwCreateWindow(windowParameters.windowWidth,
                                      windowParameters.windowHeight,
                                      windowParameters.windowTitle.c_str(),
                                      nullptr,
                                      nullptr);

        // Ensure the window is set up correctly
        if (!glfwWindow) {
            fprintf(stderr, "Could not open GLFW window\n");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        // Let the window be the current OpenGL context and initialise glad
        glfwMakeContextCurrent(glfwWindow);
        gladLoadGL();

        // Print various OpenGL information to stdout
        printf("%s: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
        printf("GLFW\t %s\n", glfwGetVersionString());
        printf("OpenGL\t %s\n", glGetString(GL_VERSION));
        printf("GLSL\t %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    ~Window() {
        glfwTerminate();
    }

    GLFWwindow* glfwWindow;
};
