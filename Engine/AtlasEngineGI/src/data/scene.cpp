#include "include/data/scene.h"
#include "include/data/model.h"

Scene::Scene(const std::string &path, GLfloat &render_time) :
    m_path(path),
    m_nb_pointlights(0),
    m_skybox(0),
    m_background_color(glm::vec3(0.f, 0.f, 0.f)),
    m_current_camera(0),
    m_render_time(render_time)
{

}

Scene::~Scene()
{
    for(GLuint i = 0; i < (GLuint)(m_roots.size()); ++i)
        delete m_roots[i];
    m_roots.clear();

    for(GLuint i = 0; i < (GLuint)(m_cameras.size()); ++i)
        delete m_cameras[i];
    m_cameras.clear();

    for(GLuint i = 0; i < (GLuint)(m_lights.size()); ++i)
        delete m_lights[i];
    m_lights.clear();

    if(m_skybox != 0)
        delete m_skybox;
}

/*
 * Draw the scene using deferred rendering
 * with the shaders of the renderer
 * each Model will be drawn using
 * its Material shader
 * */
void Scene::draw(const Shaders &shaders, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            glUseProgram(shaders[i].getProgram());
            current_camera->sendDatas(shaders[i], window_width, window_height);

            for(GLuint j = 0; j < m_lights.size(); ++j)
                m_lights[j]->sendDatas(shaders[i]);

            for(GLuint j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->draw(shaders[i], render_time);
        }
    }
}

/*
 * Draw the scene using deferred rendering
 * with one shader
 * */
void Scene::draw(const Shader &shader, const GLboolean (&keys)[1024], const GLfloat &render_time, const GLuint &window_width, const GLuint &window_height) const
{
    Camera *current_camera = m_cameras[m_current_camera];
    current_camera->orientate();
    current_camera->move(keys, render_time);

    //  Loop on every shader type
    //  Loop and draw every mesh that uses this shader
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        if(m_models[i].size() > 0)
        {
            shader.use();
            current_camera->sendDatas(shader, window_width, window_height);

            for(GLuint j = 0; j < m_lights.size(); ++j)
                m_lights[j]->sendDatas(shader);

            for(GLuint j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->draw(shader, render_time);
        }
    }
}

void Scene::buildModelList()
{
    for(GLuint i = 0; i < m_roots.size(); ++i)
        m_roots[i]->extractModels(m_models);
}

void Scene::buildKdTree()
{
    std::vector<Triangle *> T;

    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        for(GLuint j = 0, m = 0; j < m_models[i].size(); ++j)
        {
            Model* actual_model = m_models[i][j];

            //  Pour tous les meshes du model
            for(GLuint k = 0; k < actual_model->numberOfMeshes(); ++k)
            {
                Mesh* actual_mesh = actual_model->getMesh(k);
                GLuint number_of_triangles = actual_mesh->numIndices();

                //  Pour tous les triangles du mesh
                for(GLuint l = 0; l < number_of_triangles; l += 3, ++m)
                {
                    Triangle *t = actual_mesh->getTriangle(k, k+1, k+2);
                    t->mesh = j;
                    t->model = k;
                    t->side = l;

                    //  Ajout du triangle
                    m_kdtree.addTriangle(t);
                    T.push_back(t);

                    //  Recherche des extrémités de la bounding box de la scène
                    m_box.clipPoint(t->v1->Position);
                    m_box.clipPoint(t->v2->Position);
                    m_box.clipPoint(t->v3->Position);
                }
            }
        }
    }

    //  Création de la liste des évènements
    std::vector<Event> E;
    GLuint E_size;
    m_kdtree.createEventList(E_size, E);
    //  Construction de l'arbre
    m_kdtree.m_root = m_kdtree.RecBuild(T, m_box, E_size, E);
    //m_kdtree.m_root = m_kdtree.medbuild(T, B, 4);

    m_kdtree.setRootBox(m_box);

    //m_kdtree.setupMesh();   //  Affichage

    //m_kdtree_built = true;
}

void Scene::sendViewSpaceLightDatas(const Shader &shader) const
{
    for(GLuint i = 0; i < m_lights.size(); ++i)
        m_lights[i]->sendViewDatas(shader, m_cameras[m_current_camera]->getView());
}

SceneGraphNode *Scene::addSceneGraphNode(const std::string name, Model *model)
{
    SceneGraphNode *scene_graph_node = new SceneGraphNode(name);
    scene_graph_node->addModel(model);
    m_roots.back()->addChild(scene_graph_node);
    m_models[model->getMaterial()->getShaderTypeIndex()].push_back(model);

    return scene_graph_node;
}
