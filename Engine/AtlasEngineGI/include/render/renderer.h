#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <math.h>

#include "openglincludes.h"

#include "quad.h"
#include "shader.h"

#include "pipeline.h"

class Scene;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();

    void resize(const GLuint &width, const GLuint &height);

    void addPipeline(Pipeline *pipeline, const std::string &pipeline_name);

    void drawScene(const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const;

    void reloadShaders();

    //  Getter
    inline std::vector<MenuElement *> getGraphicsMenuElements() const{return m_current_pipeline->getMenuElements();}

    //  Setters
    void setCurrentPipeline(std::string pipeline_name);
    void setResolution(const GLuint &width, const GLuint &height);

private:
    void initQuad();

    GLuint m_width;
    GLuint m_height;

    Quad m_quad;

    Shader m_quad_shader;

    Pipeline *m_current_pipeline;
    std::vector<Pipeline *> m_pipelines;
    std::unordered_map<std::string, Pipeline *> m_pipelines_map;
};

#endif // RENDERER_H
