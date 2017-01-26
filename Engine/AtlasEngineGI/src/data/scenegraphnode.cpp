#include "include/data/scenegraphnode.h"
#include "include/data/camera.h"
#include "include/data/lighting/pointlight.h"

#include <string>

SceneGraphNode::SceneGraphNode(const std::string &name, glm::mat4 transform) :
    m_name(name),
    m_transform(transform),
    m_position(0.f),
    m_rotation(0.f, 0.f, 0.f, 0.f),
    m_scale(1.f)
{

}

SceneGraphNode::SceneGraphNode(const std::string &name, Model *model) :
    m_name(name),
    m_transform(glm::mat4(1)),
    m_position(0.f),
    m_rotation(0.f, 0.f, 0.f, 0.f),
    m_scale(1.f)
{
    m_models[model->getShaderTypeIndex()].push_back(model);
}

SceneGraphNode::SceneGraphNode(const std::string &name, const std::string &path, glm::mat4 global_inverse_transform) :
    m_path(path),
    m_name(name),
    m_parent(0),
    m_global_inverse_transform(global_inverse_transform),
    m_position(0.f),
    m_rotation(0.f, 0.f, 0.f, 0.f),
    m_scale(1.f)
{

}

SceneGraphNode::SceneGraphNode(const std::string &path, const glm::mat4 &global_inverse_transform, const std::string &name) :
    m_path(path),
    m_name(name),
    m_parent(0),
    m_global_inverse_transform(global_inverse_transform),
    m_position(0.f),
    m_rotation(0.f, 0.f, 0.f, 0.f),
    m_scale(1.f)
{

}

SceneGraphNode::SceneGraphNode(SceneGraphNode *parent, const std::string &name, const std::string &path, const glm::mat4 &global_inverse_transform, const glm::mat4 &transform) :
    m_path(path),
    m_name(name),
    m_parent(parent),
    m_global_inverse_transform(global_inverse_transform),
    m_node_transform(transform),
    m_position(0.f),
    m_rotation(0.f, 0.f, 0.f, 0.f),
    m_scale(1.f)
{

}

SceneGraphNode::~SceneGraphNode()
{
    //  Mesh array suppression
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
    {
        for(GLuint j = 0; j < (GLuint)(m_models[i].size()); ++j)
            delete m_models[i][j];
        m_models[i].clear();
    }

    m_models->clear();

    //  Children array suppression
    for(GLuint i = 0; i < m_children.size(); ++i)
        delete m_children[i];

    m_children.clear();
}

void SceneGraphNode::addChild(SceneGraphNode *child)
{
    child->m_path = this->m_path;
    child->m_parent = this;
    child->m_global_inverse_transform = this->m_global_inverse_transform;

    m_children.push_back(child);
}

void SceneGraphNode::setMaterial(Material* material, const std::string &name)
{
    if(m_name == name)
    {
        for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
            for(GLuint j = 0; j < m_models[i].size(); ++j)
                m_models[i][j]->setMaterial(material);
    }
    else
        for(GLuint i = 0; i < m_children.size(); ++i)
            m_children[i]->setMaterial(material, name);
}

SceneGraphNode *SceneGraphNode::getNode(const std::string &name)
{
    if(m_name == name)
        return this;
    else if(m_children.size() == 0)
            return 0;
    else
    {
        SceneGraphNode *node = 0;
        for(GLuint i = 0; i < m_children.size(); ++i)
        {
            node = m_children[i]->getNode(name);
            if(node != 0)
                return node;
        }
    }
    return 0;
}

void SceneGraphNode::spreadTransform()
{
    calculateTransform();

    for(GLuint i = 0; i < m_children.size(); ++i)
        m_children[i]->spreadTransform();
};

/*
 * Calculate node transform matrix
 * with saved transform,
 * saved rotation, saved scale and saved translation
 * And set every models of this node transforms
 * */
void SceneGraphNode::calculateTransform()
{
    m_transform = m_node_transform;
    m_transform *= glm::mat4_cast(m_rotation);
    m_transform = glm::translate(m_transform, m_position);
    m_transform = glm::scale(m_transform, m_scale);

    m_transform *= m_parent->getTransform();

    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
        for(GLuint j = 0; j < m_models[i].size(); ++j)
            m_models[i][j]->setTransform(m_transform);
}

void SceneGraphNode::extractModels(std::vector<Model *> models[NB_SHADER_TYPES])
{
    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
        for(GLuint j = 0; j < m_models[i].size(); ++j)
            models[i].push_back(m_models[i][j]);

    for(GLuint i = 0; i < m_children.size(); ++i)
        m_children[i]->extractModels(models);
}

/*****************
 * SCENEGRAPHROOT
 * ************* */

SceneGraphRoot::SceneGraphRoot(const std::string &name, const std::string &path, glm::mat4 global_inverse_transform, glm::mat4 transform) :
    SceneGraphNode(0, name, path, global_inverse_transform, transform)
{

}

SceneGraphRoot::~SceneGraphRoot()
{

}

/*
 * If name is this node name
 * translate this node with t
 * and spread to every child
 * */
void SceneGraphRoot::translate(const glm::vec3 &t, const std::string &name)
{
    SceneGraphNode *node = getNode(name);

    if(node != 0)
        node->translate(t);
}

/*
 * If name is this node name
 * rotate this node with r
 * and spread to every child
 * */
void SceneGraphRoot::rotate(const glm::vec3 &r, const std::string &name)
{
    SceneGraphNode *node = getNode(name);
    if(node != 0)
        node->rotate(glm::quat(r));
}

/*
 * If name is this node name
 * scale this node with s
 * and spread to every child
 * */
void SceneGraphRoot::scale(const glm::vec3 &s, const std::string &name)
{
    SceneGraphNode *node = getNode(name);

    if(node != 0)
        node->scale(s);
}

void SceneGraphRoot::calculateTransform()
{
    m_transform = m_node_transform;
    m_transform *= glm::mat4_cast(m_rotation);
    m_transform = glm::translate(m_transform, m_position);
    m_transform = glm::scale(m_transform, m_scale);

    for(GLuint i = 0; i < NB_SHADER_TYPES; ++i)
        for(GLuint j = 0; j < m_models[i].size(); ++j)
            m_models[i][j]->setTransform(m_transform);
}
