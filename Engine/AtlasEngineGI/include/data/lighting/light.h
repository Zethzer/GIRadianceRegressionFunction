#ifndef LIGHT_H
#define LIGHT_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Shader;

class Light
{
public:
    Light(const glm::vec3 &color, const GLfloat &intensity);

    void init(const glm::vec3 &color, const GLfloat &intensity);

    virtual void sendViewDatas(const Shader &shader, const glm::mat4 &view) const = 0;
    virtual void sendDatas(const Shader &shader) const = 0;

    //  Getters
    inline GLuint getIndex() const{return m_index;}
    inline glm::vec3 getColor() const{return m_color;}
    inline GLfloat getIntensity() const{return m_intensity;}

    //  Setter
    inline void setIndex(const GLuint &index){m_index = index;}

protected:
    GLuint m_index;
    glm::vec3 m_color;
    GLfloat m_intensity;
};

#endif // LIGHT_H
