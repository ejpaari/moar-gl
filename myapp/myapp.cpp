#include "myapp.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

MyApp::MyApp() {

}

MyApp::~MyApp() {
}

void MyApp::initialize() {
    bool isGood;
    isGood = shader.attachShader(GL_VERTEX_SHADER, "../moar-gl/myapp/shaders/test.vert");
    if (!isGood) {
        std::cout << "Failed to attach vertex shader." << std::endl;
    }

    isGood = shader.attachShader(GL_FRAGMENT_SHADER, "../moar-gl/myapp/shaders/test.frag");
    if (!isGood) {
        std::cout << "Failed to attach fragment shader." << std::endl;
    }

    shader.linkProgram();
    shader.useProgram();
    GLuint shaderProgram = shader.getProgram();

    model.setShader(shaderProgram);
    model.loadModel("../moar-gl/models/monkey.3ds");

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    uniView = glGetUniformLocation(shader.getProgram(), "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
    uniProj = glGetUniformLocation(shader.getProgram(), "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

    // Texture
    glGenTextures(1, textures);

    int width, height;
    unsigned char* image;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    image = SOIL_load_image("../moar-gl/soil-test-images/img_test.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void MyApp::run() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, model.getNumIndices(), GL_UNSIGNED_INT, 0);
}
