#include <glad/glad.h>
#include <program.hpp>
#include "glutils.h"

unsigned int generateBuffer(Mesh &mesh) {
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    unsigned int vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int normalBufferID;
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);


    return vaoID;
}

unsigned int generateBufferTexNormTBN(Mesh &mesh) {
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    unsigned int vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int normalBufferID;
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    unsigned int textureBufferID;
    glGenBuffers(1, &textureBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, textureBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.textureCoordinates.size() * sizeof(glm::vec2), mesh.textureCoordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(2);

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    std::vector<glm::vec3> tangents(mesh.vertices.size());
    std::vector<glm::vec3> bitangents(mesh.vertices.size());

    // for each face, compute the tangent and bitangent
    for (uint32_t i = 0; i < mesh.indices.size(); i += 3) {
        glm::vec3 pos1 = mesh.vertices[mesh.indices[i]];
        glm::vec3 pos2 = mesh.vertices[mesh.indices[i + 1]];
        glm::vec3 pos3 = mesh.vertices[mesh.indices[i + 2]];
        glm::vec2 uv1 = mesh.textureCoordinates[mesh.indices[i]];
        glm::vec2 uv2 = mesh.textureCoordinates[mesh.indices[i + 1]];
        glm::vec2 uv3 = mesh.textureCoordinates[mesh.indices[i + 2]];

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);

        tangents[mesh.indices[i]] = tangent;
        tangents[mesh.indices[i + 1]] = tangent;
        tangents[mesh.indices[i + 2]] = tangent;
        bitangents[mesh.indices[i]] = bitangent;
        bitangents[mesh.indices[i + 1]] = bitangent;
        bitangents[mesh.indices[i + 2]] = bitangent;
    }

    unsigned int tangentBufferID;
    glGenBuffers(1, &tangentBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBufferID);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(3);

    unsigned int bitangentBufferID;
    glGenBuffers(1, &bitangentBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentBufferID);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), bitangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(4);

    return vaoID;
}

unsigned int generateBufferTexNorm(Mesh &mesh) {
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    unsigned int vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int normalBufferID;
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    unsigned int textureBufferID;
    glGenBuffers(1, &textureBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, textureBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.textureCoordinates.size() * sizeof(glm::vec2), mesh.textureCoordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(2);

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);


    return vaoID;
}

unsigned int generateBufferTex(Mesh &mesh) {
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    unsigned int vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int textureBufferID;
    glGenBuffers(1, &textureBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, textureBufferID);
    glBufferData(GL_ARRAY_BUFFER, mesh.textureCoordinates.size() * sizeof(glm::vec2), mesh.textureCoordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(2);

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);


    return vaoID;
}
