#ifndef LIGHTINGPROCESS_H
#define LIGHTINGPROCESS_H

#include "renderprocess.h"
#include "framebuffer.h"
#include "shader.h"

class Scene;

/*
 * IN:
 * -RGBA : position + depth
 * -RGB  : normal
 * -RGBA : albedo + spec
 * -RGB  : material (roughness + ior + metalness)
 * -RED  : ambient occlusion
 *
 * OUT:
 * -RGB  : fragment color
 * -RGB  : bright color
 * -RED  : brightness
 * */

class LightingRenderProcess : public RenderProcess
{
public:
    LightingRenderProcess(const GLuint &nb_pointlights);

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement();

    virtual void resize(const GLuint &width, const GLuint &height);
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Getters
    inline Shader getShader() const{return m_shader;}

private:
    Framebuffer m_buffer;
    Shader m_shader;
};

#endif // LIGHTINGPROCESS_H
