#include <iostream>

#include "include/render/framebuffer.h"

Framebuffer::Framebuffer() :
    m_textures(0),
    m_renderbuffers(0),
    m_num_textures(0),
    m_num_renderbuffers(0)
{

}

Framebuffer::~Framebuffer()
{
    for(size_t i = 0; i < m_textures.size(); ++i)
        delete m_textures[i];
}

void Framebuffer::init(const GLuint &width, const GLuint &height)
{
    glGenFramebuffers(1, &m_buffer);
    m_width = width;
    m_height = height;
}

void Framebuffer::resize(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

    for(GLuint i = 0; i < m_num_textures; ++i)
    {
        m_textures[i]->init(m_texture_datas[i].internal_format, m_width, m_height, m_texture_datas[i].format, m_texture_datas[i].type, NULL, m_texture_datas[i].clamp, m_texture_datas[i].filter_max, m_texture_datas[i].filter_min);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->getId(), 0);
    }

    for(GLuint i = 0; i < m_num_renderbuffers; ++i)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, m_renderbuffer_datas[i].internal_format, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, m_renderbuffer_datas[i].attachment, GL_RENDERBUFFER, m_renderbuffers[i]);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer::attachTextures::Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTextures(const std::vector<FramebufferTextureDatas> &texture_datas, GLuint *custom_widths, GLuint *custom_heights)
{
    if(texture_datas.size() > 0)
    {
        m_num_textures = texture_datas.size();
        //m_textures = new Texture[m_num_textures];
        m_texture_datas = texture_datas;

        GLuint *attachments;

        if(m_num_textures > 1)
            attachments = new GLuint[m_num_textures];

        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

        for(GLuint i = 0, color_index = 0; i < m_num_textures; ++i)
        {
            GLuint width, height;
            if(custom_widths == 0 || custom_heights == 0)
            {
                width = m_width;
                height = m_height;
            }
            else
            {
                width = custom_widths[i];
                height = custom_heights[i];
            }
            //m_textures[i].init(texture_datas[i].internal_format, width, height, texture_datas[i].format, texture_datas[i].type, NULL, texture_datas[i].clamp, texture_datas[i].filter_max, texture_datas[i].filter_min);
            m_textures.push_back(new Texture(texture_datas[i].internal_format, width, height, texture_datas[i].format, texture_datas[i].type, NULL, texture_datas[i].clamp, texture_datas[i].filter_max, texture_datas[i].filter_min));

            if(texture_datas[i].is_depth)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures[i]->getId(), 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
            }
            else
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_index, GL_TEXTURE_2D, m_textures[i]->getId(), 0);

                if(m_num_textures > 1)
                    attachments[color_index++] = GL_COLOR_ATTACHMENT0 + color_index;
            }
        }

        if(m_num_textures > 1)
        {
            glDrawBuffers(m_num_textures, attachments);
            delete[] attachments;
        }

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer::attachTextures::Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Framebuffer::attachRenderBuffers(const std::vector<FramebufferRenderbufferDatas> &renderbuffer_datas, GLuint *custom_widths, GLuint *custom_heights)
{
    if(renderbuffer_datas.size() > 0)
    {
        m_num_renderbuffers = renderbuffer_datas.size();
        m_renderbuffers = new GLuint[m_num_renderbuffers];
        m_renderbuffer_datas = renderbuffer_datas;

        glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
        glGenRenderbuffers(m_num_renderbuffers, m_renderbuffers);

        for(GLuint i = 0; i < m_num_renderbuffers; ++i)
        {
            GLuint width, height;
            if(custom_widths == 0 || custom_heights == 0)
            {
                width = m_width;
                height = m_height;
            }
            else
            {
                width = custom_widths[i];
                height = custom_heights[i];
            }

            glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffers[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, renderbuffer_datas[i].internal_format, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbuffer_datas[i].attachment, GL_RENDERBUFFER, m_renderbuffers[i]);
        }

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Framebuffer::attachRenderBuffers::Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
