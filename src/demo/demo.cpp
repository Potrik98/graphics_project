#include "demo.h"
#include "model/vao.h"
#include "geometry/geometry.h"

void Demo::init() {
    shader.makeBasicShader("../res/shaders/basic.vert", "../res/shaders/basic.frag");

    root_node = new SceneNode();

    SceneNode* sphere = new SceneNode();
    root_node->children.push_back(sphere);

    VertexArrayObject sphereVao = generateSphere(1.0f, 12, 5);
    sphere->renderable.vao = sphereVao;
    sphere->is_renderable = true;
    sphere->position.z = -2;

    shader.activate();
}

void Demo::render() {
    const glm::mat4 projection_matrix = glm::perspective(
            glm::radians(90.0f),
            float(window.windowParameters.windowWidth) / float(window.windowParameters.windowHeight),
            0.1f,
            120.f);

    const glm::mat4 view_matrix(1.0);

    root_node->render(projection_matrix, view_matrix);
}

void Demo::update() {
    root_node->update(glm::mat4(1.0));
}

Demo::~Demo() {
    delete root_node;
}
