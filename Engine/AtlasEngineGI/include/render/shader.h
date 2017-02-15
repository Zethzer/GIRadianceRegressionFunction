#ifndef SHADER_H
#define SHADER_H

#include "openglincludes.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#define NB_SHADER_TYPES 16

struct ShaderType
{
    GLboolean texture;
    GLboolean normal;
    GLboolean specular;
    GLboolean animated;
    GLuint index;

    ShaderType(GLboolean t = GL_FALSE, GLboolean n = GL_FALSE, GLboolean s = GL_FALSE, GLboolean a = GL_FALSE, GLuint i = GL_FALSE) :
        texture(t),
        normal(n),
        specular(s),
        animated(a),
        index(i)
    {

    }

    void setValues(const GLboolean &t, const GLboolean &n, const GLboolean &s, const GLboolean &a, const GLuint &i)
    {
        texture = t;
        normal = n;
        specular = s;
        animated = a;
        index = i;
    }
};

struct ShaderTypes
{
    ShaderType shader_type[NB_SHADER_TYPES];

    ShaderType &operator[](GLuint i)
    {
        return shader_type[i];
    }

    const ShaderType &operator[](GLuint i) const
    {
        return shader_type[i];
    }
};

struct Define
{
    std::string text;
    GLboolean defined;

    Define(const std::string &t, const GLboolean &d) :
        text(t),
        defined(d)
    {

    }
};

ShaderType getShaderType(const GLboolean &is_animated, const GLuint &diffuse_size, const GLuint &specular_size, const GLuint &normal_size);

class Shader
{
public:
    Shader();
    Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath, std::string path = "");

    void init(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "", std::string path = "");
    void initGeometryPass(const ShaderType &shader_type);
    void initLightingPass(const GLuint &nb_pointlights);
    inline void initDefine(const std::string &index, const Define &define){m_defines.insert({index, define});}

    void reload();
    void use() const;


    //  Getters
    inline GLuint getProgram() const{return m_program;}

    //  Setters
    inline void setNbPointLights(const GLuint &nb_pointlights){m_nb_pointlights = nb_pointlights;}
    inline void setDefined(const std::string &index, const GLboolean &defined){m_defines.at(index).defined = defined;}

private:
    GLboolean compileSourceCode(const std::string &v_shader_string, const std::string &f_shader_string, const std::string &g_shader_string = "");
    GLboolean loadSourceFromFiles(std::string &vertex_code, std::string &fragment_code, std::string &geometry_code);
    void generateGeometryCode(const ShaderType &shader_type, std::string &vertex_code, std::string &fragment_code);
    void generateLightingCode(std::string &vertex_code, std::string &fragment_code);

    GLuint m_program;

    std::string m_vertex_saved_path;
    std::string m_fragment_saved_path;
    std::string m_geometry_saved_path;

    GLuint m_nb_pointlights;

    GLboolean m_initialised;

    std::unordered_map<std::string,Define> m_defines;
};

struct Shaders
{
    Shader shaders[NB_SHADER_TYPES];

    Shader &operator[](GLuint i)
    {
        return shaders[i];
    }

    const Shader &operator[](GLuint i) const
    {
        return shaders[i];
    }
};

#endif
