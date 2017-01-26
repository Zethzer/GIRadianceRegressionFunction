#ifndef SCENEGRAPHNODE_H
#define SCENEGRAPHNODE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <QtGui/QImage>

#include "include/openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "mesh.h"
#include "model.h"

class Material;
class Camera;
class PointLight;
class Animation;

struct Bone;
struct VertexBoneData;

class Texture;

class SceneGraphNode
{
    friend class Scene;
    public:
    SceneGraphNode(const std::string &name, const std::string &path, glm::mat4 global_inverse_transform = glm::mat4(1.f));
    SceneGraphNode(const std::string &path, const glm::mat4 &global_inverse_transform, const std::string &name);
    SceneGraphNode(SceneGraphNode *parent, const std::string &name, const std::string &path, const glm::mat4 &global_inverse_transform, const glm::mat4 &transform);
    SceneGraphNode(const std::string &name, glm::mat4 transform = glm::mat4(1.f));
    SceneGraphNode(const std::string &name, Model *model);
    virtual ~SceneGraphNode();
    
    void spreadTransform();
    inline void addModel(Model *model){m_models[model->getShaderTypeIndex()].push_back(model);}
    void addChild(SceneGraphNode *child);
    virtual void calculateTransform();
    
    //  Getters
    SceneGraphNode *getNode(const std::string &name);
    SceneGraphNode *getChild(const GLuint &i) const{return m_children[i];}
    inline Model *getModel(const GLuint &shader_index, const GLuint &model_index) const{return m_models[shader_index][model_index];}
    inline std::string getName() const{return m_name;}
    inline GLuint numberOfMeshes() const{return m_number_of_models;}
    inline glm::mat4 getGlobalInverseTransform() const{return m_global_inverse_transform;}
    inline glm::mat4 getTransform() const{return m_transform;}
    inline std::string getPath() const{return m_path;}
    
    //  Setters
    void setMaterial(Material *material, const std::string &name);
    inline void setParent(SceneGraphNode *parent){m_parent = parent;}
    inline void setName(const std::string &name){m_name = name;}
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}
    inline void translate(const glm::vec3 &position){m_position += position;spreadTransform();}
    inline void rotate(const glm::quat &rotation){m_rotation += rotation;spreadTransform();}
    inline void scale(const glm::vec3 &scale){m_scale *= scale;spreadTransform();}
    inline void transform(const glm::mat4 &transform){m_transform *= transform;}
    
    void extractModels(std::vector<Model *> models[NB_SHADER_TYPES]);
    
    protected:
    std::string m_path;
    std::string m_name;
    SceneGraphNode *m_parent;
    std::vector<SceneGraphNode *> m_children;
    
    std::vector<Model *> m_models[NB_SHADER_TYPES];
    GLuint m_number_of_models;
    
    glm::mat4 m_global_inverse_transform;
    glm::mat4 m_node_transform;
    glm::mat4 m_transform;
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
};

/*
 * SceneGraphRoot
 *
 * Root of the scene graph
 * Save a list of animated meshes in the scene,
 * will be useful when adding new animations
 * */
class SceneGraphRoot : public SceneGraphNode
{
    public:
    SceneGraphRoot(const std::string &name, const std::string &path, glm::mat4 global_inverse_transform = glm::mat4(1.f), glm::mat4 transform = glm::mat4(1.f));
    virtual ~SceneGraphRoot();
    
    void translate(const glm::vec3 &t, const std::string &name);
    void rotate(const glm::vec3 &r, const std::string &name);
    void scale(const glm::vec3 &s, const std::string &name);
    
    virtual void calculateTransform();
};

#endif // SCENEGRAPHNODE_H
