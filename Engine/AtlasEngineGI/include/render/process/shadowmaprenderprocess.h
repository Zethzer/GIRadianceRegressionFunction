#ifndef SHADOWMAPPROCESS_H
#define SHADOWMAPPROCESS_H

#include "renderprocess.h"
#include "framebuffer.h"
#include "shader.h"

class Scene;

/*
 * OUT:
 * -RGB  : hdr color
 * */

class ShadowMapRenderProcess : public RenderProcess
{
public:
    ShadowMapRenderProcess(const GLuint &nb_pointlights);

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement();

    virtual void resize(const GLuint &width, const GLuint &height);
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Getters
    inline Shader getShader() const{return m_shader;}

private:
    GLuint m_shadow_width;
    GLuint m_shadow_height;

    GLfloat m_near_plane;
    GLfloat m_far_plane;

    Shader m_shader;

    Framebuffer m_pointlight_depth_maps_buffer;

    GLuint m_fbo;
    GLuint m_shadowMap;
    GLuint m_depth;
};

#endif // SHADOWMAPPROCESS_H
