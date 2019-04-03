#pragma once

#include "model/vao.h"
#include <glm/glm.hpp>

class SceneNode;

class Renderable {
public:
    virtual void render(const SceneNode* sceneNode,
                        const glm::mat4& projection_matrix,
                        const glm::mat4& view_matrix);

    VertexArrayObject vao;
};
