#include "material.h"

namespace moar
{

Material::Material()
{

}

Material::~Material()
{

}

void Material::execute()
{
    // Todo: support multiple textures
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(glGetUniformLocation(shader, "myTexture"), 0);
}

} // moar
