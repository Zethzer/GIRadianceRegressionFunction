#include "include/render/process/shadowmaprenderprocess.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

ShadowMapRenderProcess::ShadowMapRenderProcess(const GLuint &nb_pointlights) :
    RenderProcess(1),
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

    const GLfloat border_color[] = {1.f, 1.f, 1.f, 1.f};

    std::vector<FramebufferTextureDatas> depth_map_texture_datas;
    depth_map_texture_datas.push_back(FramebufferTextureDatas(GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, border_color, GL_TRUE, GL_TEXTURE_CUBE_MAP));
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
    for(GLuint i = 0; i < scene.numberOfPointLights(); ++i)
    {
        glm::vec3 light_pos = scene.getPointLight(i)->getPosition();

        GLfloat aspect = m_shadow_width / m_shadow_height;
        GLfloat near = 1.0f;
        GLfloat far = 25.0f;
        glm::mat4 shadow_proj = glm::perspective(90.0f, aspect, near, far);
        std::vector<glm::mat4> shadow_transforms;
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));

        glViewport(0, 0, m_shadow_width, m_shadow_height);
        m_dirlights_depth_maps_buffer.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        m_shader.use();
        for (GLuint i = 0; i < 6; ++i)
            glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgram(), ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadow_transforms[i]));

        glUniform1f(glGetUniformLocation(m_shader.getProgram(), "far_plane"), far);
        glUniform3fv(glGetUniformLocation(m_shader.getProgram(), "lightPos"), 1, &light_pos[0]);

        scene.draw(m_shader, keys, render_time, m_shadow_width, m_shadow_height);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
