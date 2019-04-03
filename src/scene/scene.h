#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "graphics/render.h"
#include "model/vao.h"

class Renderable;

class SceneNode {
public:
    SceneNode() :
            position(0, 0, 0),
            rotation(0, 0, 0),
            scale(1, 1, 1),
            referencePoint(0, 0, 0),
            transformation_matrix(1.0),
            is_renderable(false)
            {

    }

    std::vector<SceneNode*> children;

    // The node's position and rotation relative to its parent
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 referencePoint;
    glm::mat4 transformation_matrix;
    Renderable renderable;
    bool is_renderable;

    void render(const glm::mat4& projection_matrix,
                const glm::mat4& view_matrix) {
        if (is_renderable) {
            renderable.render(this, projection_matrix, view_matrix);
        }
        for (SceneNode* child : children) child->render(projection_matrix, view_matrix);
    }

    void update(const glm::mat4& parent_transformation_matrix) {
        transformation_matrix = parent_transformation_matrix
                * glm::translate(position)
                * glm::translate(referencePoint)
                * glm::rotate(rotation.z, glm::vec3(0,0,1))
                * glm::rotate(rotation.y, glm::vec3(0,1,0))
                * glm::rotate(rotation.x, glm::vec3(1,0,0))
                * glm::translate(-referencePoint)
                * glm::scale(scale);
        for (SceneNode* child : children) child->update(transformation_matrix);
    }
};
