#include "renderobject.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <SOIL.h>
#include <iostream>

namespace moar {

RenderObject::RenderObject() {
}

RenderObject::~RenderObject() {
}

bool RenderObject::init(GLuint shaderProgram, const std::string& modelName) {
    shader = shaderProgram;
    glUseProgram(shader);

    // Model
    std::string path = "../moar-gl/models/";
    path += modelName;
    model.setShader(shader);
    bool isGood = model.loadModel(path);
    if (!isGood) {
        std::cerr << "Failed to load model: " << modelName << std::endl;
        return false;
    }

    // Todo: Texture
    glGenTextures(1, textures);

    int width, height;
    unsigned char* image;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    image = SOIL_load_image("../moar-gl/soil-test-images/img_test.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    //glUniform1i(glGetUniformLocation(shader.getProgram(), "myTexture"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Other
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    uniView = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
    uniProj = glGetUniformLocation(shader, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

    return true;
}

void RenderObject::render() {
    model.render();
}

} // moar

