#include "include/data/lighting/pointlight.h"
#include "include/render/shader.h"

PointLight::PointLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position) :
    Light(color, intensity),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f)
{
    m_color = glm::clamp(m_color, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

void PointLight::init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position)
{
    Light::init(color, intensity);

    m_position = position;
}

void PointLight::sendViewDatas(const Shader &shader, const glm::mat4 &view) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glm::vec4 position = glm::vec4(view * glm::vec4(m_position, 1));
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].intensity").c_str()), m_intensity);
}

void PointLight::sendDatas(const Shader &shader) const
{
    std::ostringstream oss;
    oss << m_index;
    std::string index = oss.str();
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].position").c_str()), m_position.x, m_position.y, m_position.z);
    glUniform3f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].color").c_str()), m_color.x, m_color.y, m_color.z);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].constant").c_str()), m_constant);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].linear").c_str()), m_linear);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].quadratic").c_str()), m_quadratic);
    glUniform1f(glGetUniformLocation(shader.getProgram(), ("pointLights[" + index + "].intensity").c_str()), m_intensity);
}
