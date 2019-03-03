#include <chrono>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <utilities/shader.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <utilities/timeutils.h>
#include <utilities/mesh.h>
#include <utilities/shapes.h>
#include <utilities/glutils.h>
#include <SFML/Audio/Sound.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gamelogic.h"
#include "sceneGraph.hpp"
#include <vector>
#include "utilities/imageLoader.hpp"
#include "utilities/glfont.h"


enum KeyFrameAction {
    BOTTOM, TOP
};

#include <timestamps.h>

double padPositionX = 0;
double padPositionY = 0;

unsigned int currentKeyFrame = 0;
unsigned int previousKeyFrame = 0;

SceneNode* rootNode;
SceneNode* boxNode;
SceneNode* ballNode;
SceneNode* padNode;

SceneNode* light_1;
SceneNode* light_2;
SceneNode* light_3;
std::vector<SceneNode*> lights;

double ballRadius = 3.0f;

// These are heap allocated, because they should not be initialised at the start of the program
sf::SoundBuffer* buffer;
Gloom::Shader* shader;
sf::Sound* sound;

const glm::vec3 boxDimensions(180, 90, 50);
const glm::vec3 padDimensions(30, 3, 40);

glm::vec3 ballPosition(0, ballRadius + padDimensions.y, boxDimensions.z / 2);
glm::vec3 ballDirection(1, 1, 0.02f);

const float BallVerticalTravelDistance = boxDimensions.y - 2.0 * ballRadius - padDimensions.y;

CommandLineOptions options;

bool hasStarted = false;
bool hasLost = false;
bool jumpedToNextFrame = false;

// Modify if you want the music to start further on in the track. Measured in seconds.
const float debug_startTime = 0;
double totalElapsedTime = debug_startTime;

uint32_t loadTextureOnGPU(const PNGImage& image) {
    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 image.width, image.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    return textureID;
}

void mouseCallback(GLFWwindow* window, double x, double y) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    padPositionX = x / double(windowWidth);
    padPositionY = y / double(windowHeight);

    if(padPositionX > 1) {
        padPositionX = 1;
        glfwSetCursorPos(window, windowWidth, y);
    } else if(padPositionX < 0) {
        padPositionX = 0;
        glfwSetCursorPos(window, 0, y);
    }
    if(padPositionY > 1) {
        padPositionY = 1;
        glfwSetCursorPos(window, x, windowHeight);
    } else if(padPositionY < 0) {
        padPositionY = 0;
        glfwSetCursorPos(window, x, 0);
    }
}

void initGame(GLFWwindow* window, CommandLineOptions gameOptions) {
    buffer = new sf::SoundBuffer();
    if (!buffer->loadFromFile("../res/Hall of the Mountain King.ogg")) {
        return;
    }

    options = gameOptions;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);

    shader = new Gloom::Shader();
    shader->makeBasicShader("../res/shaders/simple.vert", "../res/shaders/simple.frag");
    shader->activate();

    PNGImage charmap = loadPNGFile("../res/textures/charmap.png");
    uint32_t textTextureId = loadTextureOnGPU(charmap);
    Mesh text = generateTextGeometryBuffer("Hello There", 1.0f, 11.0f);
    uint32_t textVAO = generateBufferTex(text);

    SceneNode* textNode = createSceneNode();
    textNode->vertexArrayObjectID = textVAO;
    textNode->VAOIndexCount = text.indices.size();
    textNode->textureID = textTextureId;
    textNode->nodeType = GEOMETRY_2D;
    textNode->position = {0.2f, 0.8f, 0.0f};
    textNode->scale = {0.06f, 0.1f, 0.1f};

    Mesh box = generateBox(boxDimensions.x, boxDimensions.y, boxDimensions.z, true);
    Mesh pad = generateBox(padDimensions.x, padDimensions.y, padDimensions.z, false);
    Mesh sphere = generateSphere(1.0, 40, 40);

    unsigned int ballVAO = generateBuffer(sphere);
    unsigned int boxVAO = generateBufferTexNormTBN(box);
    unsigned int padVAO = generateBuffer(pad);

    PNGImage cobble_tex = loadPNGFile("../res/textures/cobble_diff.png");
    PNGImage cobble_nrm = loadPNGFile("../res/textures/cobble_normal.png");
    uint32_t cobble_texID = loadTextureOnGPU(cobble_tex);
    uint32_t cobble_nrm_texID = loadTextureOnGPU(cobble_nrm);

    rootNode = createSceneNode();
    boxNode = createSceneNode();
    padNode = createSceneNode();
    ballNode = createSceneNode();

    boxNode->nodeType = NORMAL_MAPPED_AND_TEXTURED;
    boxNode->textureID = cobble_texID;
    boxNode->normalMapTextureID = cobble_nrm_texID;

    // rootNode->children.push_back(textNode);

    light_1 = createSceneNode();
    light_2 = createSceneNode();
    light_3 = createSceneNode();
    lights.push_back(light_1);
    lights.push_back(light_2);
    lights.push_back(light_3);
    light_1->nodeType = SceneNodeType::POINT_LIGHT;
    light_2->nodeType = SceneNodeType::POINT_LIGHT;
    light_3->nodeType = SceneNodeType::POINT_LIGHT;
    ballNode->children.push_back(light_1);
    rootNode->children.push_back(light_2);
    rootNode->children.push_back(light_3);
    light_2->position.x = -100;
    light_3->position.x = 100;

    rootNode->children.push_back(boxNode);
    rootNode->children.push_back(padNode);
    rootNode->children.push_back(ballNode);

    boxNode->vertexArrayObjectID = boxVAO;
    boxNode->VAOIndexCount = box.indices.size();

    padNode->vertexArrayObjectID = padVAO;
    padNode->VAOIndexCount = pad.indices.size();

    ballNode->vertexArrayObjectID = ballVAO;
    ballNode->VAOIndexCount = sphere.indices.size();

    getTimeDeltaSeconds();

    std::cout << "Ready. Click to start!" << std::endl;
}

void updateNodeTransformations(SceneNode* node, glm::mat4 transformationThusFar) {
    glm::mat4 transformationMatrix(1.0);

    switch(node->nodeType) {
        case GEOMETRY:
        case GEOMETRY_2D:
        case NORMAL_MAPPED:
        case NORMAL_MAPPED_AND_TEXTURED:
        case TEXTURED:
            transformationMatrix =
                    glm::translate(glm::mat4(1.0), node->position)
                    * glm::translate(glm::mat4(1.0), node->referencePoint)
                    * glm::rotate(glm::mat4(1.0), node->rotation.z, glm::vec3(0,0,1))
                    * glm::rotate(glm::mat4(1.0), node->rotation.y, glm::vec3(0,1,0))
                    * glm::rotate(glm::mat4(1.0), node->rotation.x, glm::vec3(1,0,0))
                    * glm::translate(glm::mat4(1.0), -node->referencePoint)
                    * glm::scale(glm::mat4(1.0), node->scale);
            break;
        case POINT_LIGHT:
        case SPOT_LIGHT:
            transformationMatrix =
                    glm::translate(glm::mat4(1.0), node->position);
            break;
    }

    node->currentTransformationMatrix = transformationThusFar * transformationMatrix;

    for(SceneNode* child : node->children) {
        updateNodeTransformations(child, node->currentTransformationMatrix);
    }
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // printf("%.3f, %.3f %.3f\n", ballNode->position.x, ballNode->position.y, ballNode->position.z);

    double timeDelta = getTimeDeltaSeconds();

    if(!hasStarted) {

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
            if (options.enableMusic) {
                sound = new sf::Sound();
                sound->setBuffer(*buffer);
                sf::Time startTime = sf::seconds(debug_startTime);
                sound->setPlayingOffset(startTime);
                sound->play();
            }
            totalElapsedTime = debug_startTime;
            hasStarted = true;
        }

        ballPosition.x = (1 - padPositionX) * (boxDimensions.x - padDimensions.x) + padDimensions.x / 2.0;
        ballPosition.y = ballRadius + padDimensions.y;
    } else {

        // I really should calculate this using the std::chrono timestamp for this
        // You definitely end up with a cumulative error when doing lots of small additions like this
        // However, for a game that lasts only a few minutes this is fine.
        totalElapsedTime += timeDelta;

        if(hasLost) {
            ballRadius += 200 * timeDelta;
            if(ballRadius > 999999) {
                ballRadius = 999999;
            }
        } else {
            for (unsigned int i = currentKeyFrame; i < keyFrameTimeStamps.size(); i++) {
                if (totalElapsedTime < keyFrameTimeStamps.at(i)) {
                    continue;
                }
                currentKeyFrame = i;
            }

            jumpedToNextFrame = currentKeyFrame != previousKeyFrame;
            previousKeyFrame = currentKeyFrame;

            double frameStart = keyFrameTimeStamps.at(currentKeyFrame);
            double frameEnd = keyFrameTimeStamps.at(currentKeyFrame + 1); // Assumes last keyframe at infinity

            double elapsedTimeInFrame = totalElapsedTime - frameStart;
            double frameDuration = frameEnd - frameStart;
            double fractionFrameComplete = elapsedTimeInFrame / frameDuration;

            double ballYCoord;

            const float ballBottomY = ballRadius + padDimensions.y;

            KeyFrameAction currentOrigin = keyFrameDirections.at(currentKeyFrame);
            KeyFrameAction currentDestination = keyFrameDirections.at(currentKeyFrame + 1);

            if (currentOrigin == BOTTOM && currentDestination == BOTTOM) {
                ballYCoord = ballBottomY;
            } else if (currentOrigin == TOP && currentDestination == TOP) {
                ballYCoord = ballBottomY + BallVerticalTravelDistance;
            } else if (currentDestination == BOTTOM) {
                ballYCoord = ballBottomY + BallVerticalTravelDistance * (1 - fractionFrameComplete);
            } else if (currentDestination == TOP) {
                ballYCoord = ballBottomY + BallVerticalTravelDistance * fractionFrameComplete;
            }


            const float ballSpeed = 60.0f;

            ballPosition.x += timeDelta * ballSpeed * ballDirection.x;
            ballPosition.y = ballYCoord;
            ballPosition.z += timeDelta * ballSpeed * ballDirection.z;

            if (ballPosition.x + ballRadius > boxDimensions.x) {
                // Crude approximation, because it does not compute the intersection with the wall
                // Not doing it causes the ball to get stuck in the wall though
                ballPosition.x = boxDimensions.x - ballRadius;
                ballDirection.x *= -1;
            } else if (ballPosition.x - ballRadius < 0) {
                ballPosition.x = ballRadius;
                ballDirection.x *= -1;
            }

            if (ballPosition.y + ballRadius > boxDimensions.y) {
                ballPosition.y = boxDimensions.y - ballRadius;
                ballDirection.y *= -1;
            } else if (ballPosition.y - ballRadius < 0) {
                ballPosition.y = ballRadius;
                ballDirection.y *= -1;
            }

            if (ballPosition.z + ballRadius > boxDimensions.z) {
                ballPosition.z = boxDimensions.z - ballRadius;
                ballDirection.z *= -1;
            } else if (ballPosition.z - ballRadius < 0) {
                ballPosition.z = ballRadius;
                ballDirection.z *= -1;
            }

            if(options.enableAutoplay) {
                padPositionX = 1 - (ballPosition.x / (boxDimensions.x - 2 * ballRadius));
                padPositionY = 1 - (ballPosition.z / (boxDimensions.z - 2 * ballRadius));
            }

            // Check if the ball is hitting the pad when the ball is at the bottom.
            // If not, you just lost the game! (hehe)
            if (jumpedToNextFrame && currentOrigin == BOTTOM && currentDestination == TOP) {
                double padLeftXCoordinate = (1 - padPositionX) * (boxDimensions.x - padDimensions.x);
                double padRightXCoordinate = padLeftXCoordinate + padDimensions.x;

                double padFrontZCoordinate = (1 - padPositionY) * (boxDimensions.z - padDimensions.z);
                double padBackZCoordinate = padFrontZCoordinate + padDimensions.z;

                if (ballPosition.x < padLeftXCoordinate
                    || ballPosition.x > padRightXCoordinate
                    || ballPosition.z < padFrontZCoordinate
                    || ballPosition.z > padBackZCoordinate) {
                    hasLost = true;
                    if (options.enableMusic) {
                        sound->stop();
                    }
                }
            }
        }
    }

    glm::mat4 id(1);
    updateNodeTransformations(rootNode, id);

    boxNode->position = {-boxDimensions.x / 2, -boxDimensions.y / 2 - 15, boxDimensions.z - 10};
    padNode->position = {-boxDimensions.x / 2 + (1 - padPositionX) * (boxDimensions.x - padDimensions.x),
                         -boxDimensions.y / 2 - 15,
                         boxDimensions.z - 10 + (1 - padPositionY) * (boxDimensions.z - padDimensions.z)};
    ballNode->position = {-boxDimensions.x / 2 + ballPosition.x,
                          -boxDimensions.y / 2 - 15 + ballPosition.y,
                          boxDimensions.z - 10 + ballPosition.z};

    ballNode->scale = {ballRadius, ballRadius, ballRadius};
}

void enable_texture(const SceneNode* node) {
    glUniform1i(13, 1); // is_textured = true
    glBindTextureUnit(0, node->textureID);
}

void enable_normal_map(const SceneNode* node) {
    glUniform1i(15, 1); // is_normal_mapped = true
    glBindTextureUnit(1, node->normalMapTextureID);
}

void enable_light(const SceneNode* node) {
    glUniform1i(14, 1); // is_lighted = true
    // Spot light
    glm::vec3 light_pos = glm::vec3(0,100,0);
    glm::vec3 light_dir = glm::normalize(ballNode->position - light_pos);
    float angle_cos = 0.93969262078f; // 20 degrees
    glUniform3fv(9, 1, glm::value_ptr(light_pos));
    glUniform3fv(10, 1, glm::value_ptr(light_dir));
    glUniform1fv(11, 1, &angle_cos);

    // ball position
    glUniform3fv(12, 1, glm::value_ptr(ballNode->position));

    for (int i = 6; i < 9; ++i) {
        glUniform3fv(i, 1, glm::value_ptr(
            glm::vec3(lights[i - 6]->currentTransformationMatrix[3])));
            // the translation part of the transformation matrix is the 4th vector,
            // and the position of the light in world coordinates is this translation.
    }

    // Normal matrix
    glm::mat4 N = glm::transpose(glm::inverse(node->currentTransformationMatrix));
    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(N));
}

void renderNode(SceneNode* node) {
    switch (node->nodeType)
    {
        case GEOMETRY:
            glUniform1i(13, 0); // is_textured = false
            enable_light(node);
            glUniform1i(15, 0); // is_normal_mapped = false
            break;
        case TEXTURED:
            enable_texture(node);
            enable_light(node);
            glUniform1i(15, 0); // is_normal_mapped = false
            break;
        case NORMAL_MAPPED:
            glUniform1i(13, 0); // is_textured = false
            enable_light(node);
            enable_normal_map(node);
            break;
        case NORMAL_MAPPED_AND_TEXTURED:
            enable_texture(node);
            enable_light(node);
            enable_normal_map(node);
            break;
        case GEOMETRY_2D:
            enable_texture(node);
            glUniform1i(14, 0); // is_lighted = false
            glUniform1i(15, 0); // is_normal_mapped = false
            break;
    
        default:
            break;
    }
    if (node->nodeType == GEOMETRY_2D) {
        // printf("rendering text\n");
        glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f);
        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));
        glBindTextureUnit(0, node->textureID);
    } else {
        // printf("rendering node\n");
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(windowWidth) / float(windowHeight), 0.1f,
                                                120.f);

        glm::mat4 cameraTransform =   glm::translate(glm::mat4(1), glm::vec3(0, 0, 0))
                                    * glm::rotate(glm::mat4(1.0), 0.2f, glm::vec3(1, 0, 0))
                                    * glm::rotate(glm::mat4(1.0), float(M_PI), glm::vec3(0, 1, 0));

        glm::mat4 VP = projection * cameraTransform;

        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(VP));
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));
    }
    switch(node->nodeType) {
        case GEOMETRY:
        case GEOMETRY_2D:
        case TEXTURED:
        case NORMAL_MAPPED:
        case NORMAL_MAPPED_AND_TEXTURED:
            if(node->vertexArrayObjectID != -1) {
                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
            }
            break;
        case POINT_LIGHT:
        case SPOT_LIGHT:
            break;
    }

    // printf("done\n");

    for(SceneNode* child : node->children) {
        renderNode(child);
    }
}

void renderFrame(GLFWwindow* window) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    renderNode(rootNode);
}
