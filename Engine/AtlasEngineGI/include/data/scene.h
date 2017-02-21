#ifndef SCENE_H
#define SCENE_H

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "openglincludes.h"

#include "include/data/scenegraphnode.h"
#include "include/render/shader.h"
#include "include/data/camera.h"
#include "include/data/lighting/pointlight.h"
#include "include/data/geometry/kdtree.h"

#include <string>

class Model;
class Skybox;

class Scene
{
    public:
    Scene(const std::string &path, GLfloat &render_time);
    ~Scene();

    void draw(const Shaders &shaders, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const;
    void draw(const Shader &shader, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const;

    //  Add
    SceneGraphNode *addSceneGraphNode(const std::string name, Model *model);
    inline void addSceneGraphRoot(SceneGraphRoot *s){m_roots.push_back(s);}
    inline void addPointLight(PointLight *light){light->setIndex(m_nb_pointlights++);m_lights.push_back(light); m_point_lights.push_back(light); m_current_pointlight = light;}
    inline void addCamera(Camera *c){m_cameras.push_back(c);}
    inline void addModel(Model *model){m_models[model->getShaderTypeIndex()].push_back(model);}

    //  Send
    void sendViewSpaceLightDatas(const Shader &shader) const;
    inline void sendCameraToShader(const Shader &shader, const GLfloat &screen_width, const GLfloat &screen_height) const{m_cameras[m_current_camera]->sendDatas(shader, screen_width, screen_height);}

    //  Getters
    inline SceneGraphRoot *getLastRoot()const{return m_roots.back();}
    inline glm::vec3 getBackgroundColor() const{return m_background_color;}
    inline std::string getPath() const{return m_path;}
    inline Camera *getCurrentCamera() const{return m_cameras[m_current_camera];}
    inline KdTree getKdTree() const{return m_kdtree;}
    inline Light *getLight(const GLuint &i) const{return m_lights[i];}
    inline PointLight *getPointLight(const GLuint &i) const{return m_point_lights[i];}
    inline AABB getAABB() const{return m_AABB;}

    inline GLuint numberOfPointLights() const{return m_nb_pointlights;}
    inline GLuint numberOfModels(const GLuint &shader_index) const{return m_models[shader_index].size();}

    //  Others
    inline void translate(const glm::vec3 &t, const std::string &node_name){for(GLuint i = 0; i < m_roots.size(); ++i)m_roots[i]->translate(t, node_name); buildAABB();}
    inline void rotate(const glm::vec3 &r, const std::string &node_name){for(GLuint i = 0; i < m_roots.size(); ++i)m_roots[i]->rotate(glm::radians(r), node_name); buildAABB();}
    inline void scale(const glm::vec3 &s, const std::string &node_name){for(GLuint i = 0; i < m_roots.size(); ++i)m_roots[i]->scale(s, node_name); buildAABB();}

    inline void updateCamera(const GLfloat &xoffset, const GLfloat &yoffset){m_cameras[m_current_camera]->setOffset(xoffset, yoffset);}
    inline void moveLights(const GLboolean keys[1024], const GLfloat &delta_time){for(size_t i = 0; i < m_point_lights.size(); ++i)m_point_lights[i]->move(keys, delta_time, m_AABB);}
    
    void buildModelList();
    void buildKdTree();
    void buildAABB();


    protected:
    std::string m_path;
    std::vector<Light *> m_lights;
    std::vector<PointLight *> m_point_lights;
    std::vector<Camera *> m_cameras;
    std::vector<SceneGraphRoot *> m_roots;
    std::vector<Model *> m_models[NB_SHADER_TYPES];

    PointLight *m_current_pointlight;

    GLuint m_nb_pointlights;

    Skybox *m_skybox;
    glm::vec3 m_background_color;

    GLuint m_current_camera;

    GLfloat &m_render_time;

    AABB m_AABB;
    KdTree m_kdtree;
};

#endif // SCENE_H
