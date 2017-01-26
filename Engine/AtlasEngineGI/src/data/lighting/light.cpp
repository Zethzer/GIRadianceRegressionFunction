#include "include/data/lighting/light.h"

Light::Light(const glm::vec3 &color, const GLfloat &intensity) :
    m_index(0),
    m_color(color),
    m_intensity(intensity)
{

}

void Light::init(const glm::vec3 &color, const GLfloat &intensity)
{
    m_color = color;
    m_intensity = intensity;
}
