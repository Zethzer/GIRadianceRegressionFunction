#ifndef HDRPROCESS_H
#define HDRPROCESS_H

#include <QObject>

#include "openglincludes.h"

#include "renderprocess.h"
#include "shader.h"
#include "framebuffer.h"

class Scene;
class Quad;

/*
 * IN:
 * -RGB  : ldr color
 * -RGB  : bright color
 * -RED  : brightness
 *
 * OUT:
 * -RGB  : hdr color
 * */
class HDRRenderProcess : public QObject, public RenderProcess
{
    Q_OBJECT
public:
    HDRRenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement();

    virtual void resize(const GLuint &width, const GLuint &height);
    void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]);

    //  Setters
    virtual void setActivated(const GLboolean &activated);

private slots:
    void switchHDR();
    void switchbloom();
    void switchAdaptation();
    void switchChromaticAberration();

private:
    void processAdaptation(const Quad &quad, const GLfloat &render_time);
    void processBloom(const Quad &quad);

    Shader m_HDR_shader;
    GLboolean m_HDR;
    GLboolean m_adaptation;
    GLboolean m_chromatic_aberration;

    //  BLOOM
    Framebuffer m_blur_buffers[2];
    Shader m_blur_shaders[3];
    GLboolean m_bloom;
    GLuint m_bloom_quality;
    GLuint m_buffer_index;

    //ADAPTATION
    Framebuffer m_brightness_ping_buffer;
    Framebuffer m_brightness_ping_buffer2;
    Framebuffer m_brightness_pong_buffer;
    Framebuffer m_brightness_pong_buffer2;
    Shader m_downscaling_shader;
    GLfloat m_saved_avg_luminance;

    Framebuffer m_out_buffer;

    GLint m_avg_lum_location;
    GLint m_max_lum_location;
};

#endif // HDRPROCESS_H
