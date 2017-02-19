#include "include/data/lighting/pointlight.h"
#include "include/render/shader.h"
#include "include/data/geometry/mesh.h"

PointLight::PointLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position) :
    Light(color, intensity),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.7f),
    m_quadratic(1.8f),
    m_speed(0.005f)
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

void PointLight::move(const GLboolean keys[], const GLfloat &delta_time, const AABB &box)
{
    float speed = m_speed * delta_time;
    glm::vec3 new_pos = m_position;

    if(keys[Qt::Key_O])
        new_pos.x += speed;
    if(keys[Qt::Key_L])
        new_pos.x -= speed;
    if(keys[Qt::Key_I])
        new_pos.y -= speed;
    if(keys[Qt::Key_P])
        new_pos.y += speed;
    if(keys[Qt::Key_K])
        new_pos.z -= speed;
    if(keys[Qt::Key_M])
        new_pos.z += speed;

    //if(box.isPointInside(new_pos))
        m_position = new_pos;
}
