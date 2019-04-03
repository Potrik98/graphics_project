#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "mesh.h"

/*
 * Class for handling vertex array objects
 * with an arbitrary amount of vertex attributes.
 * Handles vao generation, binding of index and vertex buffers,
 * and rendering of the vao.
 *
 * Example usage:
 * VertexArrayObject vao(vertex_count);
 * vao.indexBuffer(indices, index_count) // binds the indices to the vao
 *   ->vertexBuffer(vertices, 3, 0); // binds vertices to location 0, with component count 3
 *
 * vao.render(); // renders the vao.
 */
class VertexArrayObject {
public:
    VertexArrayObject(const Mesh& mesh) :
            m_index_count(0),
            m_vertex_count(0) {
        indexBuffer(mesh.indices);
        vertexBuffer<3>(mesh.vertices, 0);
        vertexBuffer<2>(mesh.textureCoordinates, 1);
        vertexBuffer<3>(mesh.normals, 2);
    }

    VertexArrayObject() :
            m_index_count(0),
            m_vertex_count(0) {
        glGenVertexArrays(1, &m_id);
    }

    /*
     * Binds an index array to the vao.
     * The vao will use this index array when rendering.
     */
    VertexArrayObject* indexBuffer(const unsigned int* indices,
                                   const unsigned int& index_count) {
        m_index_count = index_count;
        glBindVertexArray(m_id);

        unsigned int idx_buf_id;
        glGenBuffers(1, &idx_buf_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buf_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(int), indices, GL_STATIC_DRAW);

        return this;
    }

    /*
     * Binds an index array to the vao.
     * The vao will use this index array when rendering.
     */
    VertexArrayObject* indexBuffer(const std::vector<unsigned int>& indices) {
        return this->indexBuffer(indices.data(), static_cast<const unsigned int&>(indices.size()));
    }

    /*
     * Binds a vertex array to the vao in the given location.
     * Takes in the vertex data, component count and location.
     */
    VertexArrayObject* vertexBuffer(const float* vertices,
                                    const unsigned int& vertex_count,
                                    const unsigned int& component_count,
                                    const unsigned int& location) {
        m_vertex_count = vertex_count;

        glBindVertexArray(m_id);

        unsigned int vtx_buf_id;
        glGenBuffers(1, &vtx_buf_id);

        glBindBuffer(GL_ARRAY_BUFFER, vtx_buf_id);
        glBufferData(
                GL_ARRAY_BUFFER,
                vertex_count * component_count * sizeof(float),
                vertices,
                GL_STATIC_DRAW
        );

        // Set the attrib pointer for the location
        glVertexAttribPointer(
                location,
                component_count,
                GL_FLOAT, // float data
                GL_FALSE,
                0,
                nullptr
        );

        // Enable attributes for the location
        glEnableVertexAttribArray(location);

        return this;
    }

    VertexArrayObject* vertexBuffer(const float* vertices,
                                    const unsigned int& component_count,
                                    const unsigned int& location) {
        return this->vertexBuffer(vertices,
                                  m_vertex_count,
                                  component_count,
                                  location);
    }

    /*
     * Binds a vertex array with a given number of components to the vao in the given location.
     * Takes in the vertex data and location.
     */
    template <unsigned int component_count>
    VertexArrayObject* vertexBuffer(const std::vector<glm::vec<component_count, float, glm::defaultp>> vertices,
                                    const unsigned int& location) {
        const auto vertex_count = static_cast<const unsigned int>(vertices.size());
        return this->vertexBuffer(reinterpret_cast<const float*>(vertices.data()),
                                  vertex_count,
                                  component_count,
                                  location);
    }

    /*
     * Render the vao
     */
    void render() {
        // TODO: enable rendering of VAOs without index buffers
        glBindVertexArray(m_id);
        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
    }

private:
    GLuint m_id{};
    unsigned int m_index_count;
    unsigned int m_vertex_count;
};
