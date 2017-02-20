#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "shader.h"

class ComputeShader
{
public:
    ComputeShader();
    ComputeShader(const std::string &computeShader, std::string path = "");

    void init(const std::string &computeShader, std::string path = "");

    void reload();
    void use() const;

    // Getters
    inline GLuint getProgram() const {return m_program;}

private:
    GLboolean compileSourceCode(const std::string &compute_shader_string);
    GLboolean loadSourceFromFiles(std::string &computeshader_code);

    GLuint m_program;

    std::string m_computeshader_saved_path;

    GLuint m_initialised;
};

#endif // COMPUTESHADER_H
