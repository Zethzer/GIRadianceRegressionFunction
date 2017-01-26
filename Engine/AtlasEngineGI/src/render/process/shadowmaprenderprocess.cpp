#include "include/render/process/shadowmaprenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

ShadowMapRenderProcess::ShadowMapRenderProcess(const GLuint &nb_pointlights) :
    RenderProcess::RenderProcess(1),
    m_shadow_width(1024),
    m_shadow_height(1024),
    m_near_plane(1.f),
    m_far_plane(10.f)
{

}

void ShadowMapRenderProcess::init(const GLuint &width, const GLuint &height)
{
    RenderProcess::init(m_shadow_width, m_shadow_height);

    m_dirlights_depth_maps_buffer.init(m_shadow_width, m_shadow_height);

    std::vector<FramebufferTextureDatas> depth_map_texture_datas;
    depth_map_texture_datas.push_back(FramebufferTextureDatas(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_NEAREST, GL_NEAREST, {1.f, 1.f, 1.f, 1.f}, GL_TRUE));
    m_dirlights_depth_maps_buffer.attachTextures(depth_map_texture_datas);

    m_shader.init("shaders/depth.vert", "shaders/depth.frag");

    m_out_textures.push_back(m_dirlights_depth_maps_buffer.getTexture(0));
}

void ShadowMapRenderProcess::initMenuElement()
{

}

void ShadowMapRenderProcess::resize(const GLuint &width, const GLuint &height)
{

}

void ShadowMapRenderProcess::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024])
{
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    GLfloat near_plane = 1.0f, far_plane = 20.0f;
    lightProjection = glm::ortho(-7.0f, 7.0f, -7.0f, 7.0f, near_plane, far_plane);
    //lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
    lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    m_shader.use();
    glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgram(), "light_space_matrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glViewport(0, 0, m_shadow_width, m_shadow_height);
    m_dirlights_depth_maps_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.draw(m_shader, keys, render_time, m_shadow_width, m_shadow_height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
