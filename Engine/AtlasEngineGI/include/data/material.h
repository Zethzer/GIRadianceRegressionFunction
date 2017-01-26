#ifndef MATERIAL_H
#define MATERIAL_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "texture.h"

#include <unordered_map>

class Material
{
    public:
    Material();
    Material(const glm::vec3 &color, const GLfloat &roughness, const GLfloat &metalness, const GLfloat &refraction, const GLfloat &opacity, GLboolean has_normal_map = GL_FALSE, std::vector<Texture *> textures = std::vector<Texture *>(), ShaderType shader_type = ShaderType());
    Texture *textureFromFile(const std::string &directory, const GLchar *path, const std::string &shading_type) const;
    
    void sendDatas(const Shader &shader) const;
    
    //  Getters
    inline GLboolean hasNormalMap() const{return m_has_normal_map;}
    inline GLuint getShaderTypeIndex() const{return m_shader_type.index;}
    
    //  Setters
    void copy(const Material &material);
    inline void copy(const Material *material){copy(*material);}
    
    private:
    glm::vec3 m_color;
    GLfloat m_roughness;
    GLfloat m_metalness;
    GLfloat m_refraction;
    GLfloat m_opacity;
    
    GLboolean m_has_normal_map;
    
    std::vector<Texture *> m_textures;
    
    ShaderType m_shader_type;
};

struct MaterialLibrary
{
    std::unordered_map<std::string, Material *> materials;
    std::unordered_map<std::string, Texture *> textures_loaded;
    
    ~MaterialLibrary()
    {
        for(std::unordered_map<std::string, Material *>::iterator it = materials.begin(); it != materials.end(); ++it)
        delete it->second;
        
        materials.clear();
        
        for(std::unordered_map<std::string, Texture *>::iterator it = textures_loaded.begin(); it != textures_loaded.end(); ++it)
        delete it->second;
        
        textures_loaded.clear();
    }
    
    void addMaterial(Material *material, const std::string &name)
    {
        if(materials.find(name) == materials.end())
        materials[name] = material;
    }
    
    inline Material *getMaterial(const std::string &name) const{return materials.at(name);}
};

#endif // MATERIAL_H
