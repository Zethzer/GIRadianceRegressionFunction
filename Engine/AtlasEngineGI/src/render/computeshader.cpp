#include "computeshader.h"

ComputeShader::ComputeShader() :
    m_initialised(GL_FALSE)
{

}

ComputeShader::ComputeShader(const std::string &computeShader, std::string path) :
    m_computeshader_saved_path(computeShader),
    m_initialised(GL_FALSE)
{
    m_computeshader_saved_path = path + m_computeshader_saved_path;

    std::string computeShader_code;

    loadSourceFromFiles(computeShader_code);
    compileSourceCode(computeShader_code.c_str());

    m_initialised = GL_TRUE;
}

void ComputeShader::init(const std::string &computeShader, std::string path)
{
    m_computeshader_saved_path = computeShader;

//    m_computeshader_saved_path = path + m_computeshader.path;

    std::string computeShader_code;

    loadSourceFromFiles(computeShader_code);
    compileSourceCode(computeShader_code.c_str());

    m_initialised = GL_TRUE;
}

void ComputeShader::reload()
{
    std::string computeShader_code;

    loadSourceFromFiles(computeShader_code);
    compileSourceCode(computeShader_code.c_str());
}

void ComputeShader::use() const
{
    if(m_initialised)
        glUseProgram(m_program);
    else
        std::cerr << "ERROR::SHADER::USE The shader has to be initialised before it can be used" << std::endl;
}

GLboolean ComputeShader::compileSourceCode(const std::string &compute_shader_string)
{
    GLuint compute;
    GLint success;
    GLchar info_log[512];

    const GLchar *compute_shader_code = compute_shader_string.c_str();

    /*
     * Compute shader compilation
     * */
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &compute_shader_code, NULL);
    glCompileShader(compute);

    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(compute, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << m_computeshader_saved_path << " " << info_log << std::endl;
        return GL_FALSE;
    }

    /*
     * Program linking
     * */
    if(m_initialised)
        glDeleteProgram(m_program);
    m_program = glCreateProgram();
    glAttachShader(m_program, compute);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_program, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << m_computeshader_saved_path << " " << info_log << std::endl;
        return GL_FALSE;
    }

    glDeleteShader(compute);

    return GL_TRUE;
}

GLboolean ComputeShader::loadSourceFromFiles(std::string &computeshader_code)
{
    std::ifstream computeshader_file;

    computeshader_file.exceptions(std::ifstream::badbit);

    try
    {
        computeshader_file.open(m_computeshader_saved_path.c_str());

        std::stringstream computeshader_stream;

        computeshader_stream << computeshader_file.rdbuf();

        computeshader_file.close();

        computeshader_code = computeshader_stream.str();
    }

    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << m_computeshader_saved_path << " " << std::endl;
        return GL_FALSE;
    }

    return GL_TRUE;
}
