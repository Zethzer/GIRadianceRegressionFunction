#include "include/render/process/lightingrenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

LightingRenderProcess::LightingRenderProcess(const GLuint &nb_pointlights) :
    RenderProcess(5)
{
    m_shader.initLighting(nb_pointlights);
}

void LightingRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(width, height);

    m_buffer.init(width, height);
    std::vector<FramebufferTextureDatas> lighting_textures_datas;
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER));
    lighting_textures_datas.push_back(FramebufferTextureDatas(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER));
    m_buffer.attachTextures(lighting_textures_datas);

    m_shader.use();
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "gMaterial"), 3);
    glUniform1i(glGetUniformLocation(m_shader.getProgram(), "AO"), 4);
    glUseProgram(0);

    m_out_textures.push_back(m_buffer.getTexture(0));
    m_out_textures.push_back(m_buffer.getTexture(1));
    m_out_textures.push_back(m_buffer.getTexture(2));
}

void LightingRenderProcess::initMenuElement()
{

}

void LightingRenderProcess::resize(const GLuint &width, const GLuint &height)
{
    RenderProcess::resize(width, height);

    m_buffer.resize(width, height);
}

void LightingRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    m_buffer.bind();
    glViewport(0, 0, m_buffer.width(), m_buffer.height());
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader.use();

    glActiveTexture(GL_TEXTURE0);
    bindPreviousTexture(0);
    glActiveTexture(GL_TEXTURE1);
    bindPreviousTexture(1);
    glActiveTexture(GL_TEXTURE2);
    bindPreviousTexture(2);
    glActiveTexture(GL_TEXTURE3);
    bindPreviousTexture(3);
    glActiveTexture(GL_TEXTURE4);
    bindPreviousTexture(4);


    scene.sendViewSpaceLightDatas(m_shader);
    scene.sendCameraToShader(m_shader, m_buffer.width(), m_buffer.height());

    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    GLfloat near_plane = 1.0f, far_plane = 20.0f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    //lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
    lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView * glm::inverse(scene.getCurrentCamera()->getView());

    glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgram(), "light_space_matrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    quad.draw();
}
