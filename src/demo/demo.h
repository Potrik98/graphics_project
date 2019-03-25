#pragma once

#include <glad/glad.h>
#include "core/program.h"

class Demo : public Program {
public:
    Demo(GLFWwindow* window) : Program(window) {}

    void update() override;

    void render() override;

    void init() override;
};
