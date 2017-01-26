#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "openglincludes.h"
#include "light.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class PointLight : public Light
{
public:
    PointLight(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position);

    void init(const glm::vec3 &color, const GLfloat &intensity, const glm::vec3 &position);

    virtual void sendViewDatas(const Shader &shader, const glm::mat4 &view) const;
    virtual void sendDatas(const Shader &shader) const;

    //  Getters
    inline glm::vec3 getPosition() const{return m_position;}
    inline GLfloat getConstant() const{return m_constant;}
    inline GLfloat getLinear() const{return m_linear;}
    inline GLfloat getQuadratic() const{return m_quadratic;}

protected:
    glm::vec3 m_position;

    GLfloat m_constant;
    GLfloat m_linear;
    GLfloat m_quadratic;
};

#endif // POINTLIGHT_H
