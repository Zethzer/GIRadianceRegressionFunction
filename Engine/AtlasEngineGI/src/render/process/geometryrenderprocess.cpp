#include "include/render/process/geometryrenderprocess.h"
#include "include/data/scene.h"

GeometryRenderProcess::GeometryRenderProcess() :
    RenderProcess::RenderProcess(0)
{

}

void GeometryRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(width, height);

    m_shader_types[0].setValues(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 0);
    m_shader_types[1].setValues(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, 1);
    m_shader_types[2].setValues(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE, 2);
    m_shader_types[3].setValues(GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, 3);
    m_shader_types[4].setValues(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, 4);
    m_shader_types[5].setValues(GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE, 5);
    m_shader_types[6].setValues(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE, 6);
    m_shader_types[7].setValues(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE, 7);

    m_shader_types[8].setValues(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE, 8);
    m_shader_types[9].setValues(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, 9);
    m_shader_types[10].setValues(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, 10);
    m_shader_types[11].setValues(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE, 11);
    m_shader_types[12].setValues(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE, 12);
    m_shader_types[13].setValues(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE, 13);
    m_shader_types[14].setValues(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE, 14);
    m_shader_types[15].setValues(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, 15);

    m_gBuffer.init(width, height);
    std::vector<FramebufferTextureDatas> gTexture_datas;
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE));
    gTexture_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE));
    m_gBuffer.attachTextures(gTexture_datas);
    std::vector<FramebufferRenderbufferDatas> gRenderbuffer_datas;
    gRenderbuffer_datas.push_back(FramebufferRenderbufferDatas(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT));
    m_gBuffer.attachRenderBuffers(gRenderbuffer_datas);

    // Setup shaders
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
        m_shaders[i].initGeometry(m_shader_types[i]);

    m_out_textures.push_back(m_gBuffer.getTexture(0));
    m_out_textures.push_back(m_gBuffer.getTexture(1));
    m_out_textures.push_back(m_gBuffer.getTexture(2));
    m_out_textures.push_back(m_gBuffer.getTexture(3));
}

void GeometryRenderProcess::initMenuElement()
{

}

void GeometryRenderProcess::resize(const GLuint &width, const GLuint &height)
{
    RenderProcess::resize(width, height);

    m_gBuffer.resize(width, height);
}

void GeometryRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    glViewport(0, 0, m_gBuffer.width(), m_gBuffer.height());
    m_gBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.draw(m_shaders, keys, render_time, m_width, m_height);
}
