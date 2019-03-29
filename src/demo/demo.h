#pragma once

#include <glad/glad.h>
#include "core/program.h"
#include "graphics/shader.h"

class Demo : public Program {
public:
    Demo() : Program({
            1920,
            1080,
            4,
            GL_TRUE,
            "Graphics project demo",
    }) {}

    void update() override;

    void render() override;

    void init() override;

    Shader shader;
};
