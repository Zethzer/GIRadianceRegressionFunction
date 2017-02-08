#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>

#include "openglincludes.h"
#include "texture.h"

struct FramebufferTextureDatas
{
    GLint internal_format;
    GLenum format;
    GLenum type;
    GLint clamp;
    GLuint filter_min;
    GLuint filter_max;
    GLfloat border_color[4];
    GLboolean is_depth;
    GLenum target;

    FramebufferTextureDatas(const GLint &internal_format_, const GLenum &format_, const GLenum &type_, GLint clamp_ = GL_FALSE, GLuint filter_min_ = GL_NEAREST, GLuint filter_max_ = GL_NEAREST, const GLfloat border_color_[4] = default_border_color, GLboolean is_depth_ = GL_FALSE, GLenum target_ = GL_TEXTURE_2D) :
        internal_format(internal_format_),
        format(format_),
        type(type_),
        clamp(clamp_),
        filter_min(filter_min_),
        filter_max(filter_max_),
        is_depth(is_depth_),
        target(target_)
    {
        border_color[0] = border_color_[0];
        border_color[1] = border_color_[1];
        border_color[2] = border_color_[2];
        border_color[3] = border_color_[3];
    }
};

struct FramebufferRenderbufferDatas
{
    GLint internal_format;
    GLenum attachment;

    FramebufferRenderbufferDatas(const GLenum &i_f, const GLenum &at) :
        internal_format(i_f),
        attachment(at)
    {
    }
};

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void init(const GLuint &width, const GLuint &height);
    void resize(const GLuint &width, const GLuint &height);

    void attachTextures(const std::vector<FramebufferTextureDatas> &texture_datas, GLuint *custom_widths = 0, GLuint *custom_heights = 0);
    void attachRenderBuffers(const std::vector<FramebufferRenderbufferDatas> &renderbuffer_datas, GLuint *custom_widths = 0, GLuint *custom_heights = 0);

    inline void bind(GLenum target = GL_FRAMEBUFFER)const {glBindFramebuffer(target, m_buffer);}

    //  Getters
    inline GLuint width() const{return m_width;}
    inline GLuint height() const{return m_height;}
    inline GLuint getBuffer() const{return m_buffer;}
    inline Texture *getTexture(const GLuint &i) const{return m_textures[i];}

private:
    GLuint m_buffer;
    std::vector<Texture *> m_textures;
    GLuint *m_renderbuffers;

    GLuint m_num_textures;
    GLuint m_num_renderbuffers;

    std::vector<FramebufferTextureDatas> m_texture_datas;
    std::vector<FramebufferRenderbufferDatas> m_renderbuffer_datas;

    GLuint m_width;
    GLuint m_height;
};

#endif // FRAMEBUFFER_H
