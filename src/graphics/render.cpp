#include "render.h"
#include "scene/scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

void Renderable::render(const SceneNode* sceneNode,
                        const glm::mat4& projection_matrix,
                        const glm::mat4& view_matrix) {
    const glm::mat4 M = projection_matrix * view_matrix * sceneNode->transformation_matrix;
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(M));

    vao.render();
}
