#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/quaternion.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "mesh.h"
#include "material.h"

#define NUM_BONES_PER_VERTEX 4

class Camera;
class PointLight;

class Model
{
    public:
    Model();
    Model(Mesh *mesh, Material *material);
    virtual ~Model();
    
    virtual void draw(const Shader &shader, const GLfloat &render_time) const;
    
    //  Getters
    inline GLuint getShaderTypeIndex() const{return m_material->getShaderTypeIndex();}
    Mesh *getMesh(const GLuint &index) const{return m_meshes[index];}
    Material *getMaterial() const{return m_material;}
    inline GLuint numberOfMeshes() const{return m_meshes.size();}
    
    //  Setters
    inline void setTransform(const glm::mat4 &transform){m_transform = transform;}
    inline void setMaterial(Material *material){m_material = material;}
    
    protected:
    std::vector<Mesh *> m_meshes;
    
    Material *m_material;
    ShaderType m_shader_type;
    
    glm::mat4 m_transform;
};

#endif // MODEL_H
