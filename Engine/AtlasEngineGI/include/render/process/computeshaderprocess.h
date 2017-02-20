#ifndef COMPUTESHADERPROCESS_H
#define COMPUTESHADERPROCESS_H

#include "renderprocess.h"
#include "framebuffer.h"
#include "shader.h"
#include "computeshader.h"

#include "openglincludes.h"


/* Structure input buffer test
 *
 * */

struct couleurStruct {
    double r;
    double g;
    double b;
    double a;
};

/*
 * OUT:
 * -RGBA : position + depth
 * */

class ComputeShaderProcess : public RenderProcess
{
public:
    ComputeShaderProcess();

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement();

    virtual void resize(const GLuint &width, const GLuint &height);
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    // Getters
    inline ComputeShader getShader() const{return m_shader;}

private:

    ComputeShader m_shader;

    Texture m_out_texture;
    Texture m_out_texture2;

    int m_tex_w;
    int m_tex_h;
};

#endif // COMPUTESHADERPROCESS_H
