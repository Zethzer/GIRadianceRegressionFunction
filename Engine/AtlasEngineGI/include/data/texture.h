#ifndef TEXTURE_H
#define TEXTURE_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class Texture
{
public:
    Texture();
    Texture(const GLint &internal_format, const GLsizei &width, const GLsizei &height, const GLenum &format, const GLenum &type, const GLvoid *data, GLuint clamp = GL_FALSE, GLuint filter_min = GL_NEAREST, GLuint filter_max = GL_NEAREST, std::string path = "", std::string shading_type = "", glm::vec4 border_color = glm::vec4(0.f), GLenum target = GL_TEXTURE_2D);
    Texture(const std::string &directory, const GLchar *path, const std::string &shading_type);
    void init(const GLint &internal_format, const GLsizei &width, const GLsizei &height, const GLenum &format, const GLenum &type, const GLvoid *data, GLuint clamp = GL_FALSE, GLuint filter_min = GL_NEAREST, GLuint filter_max = GL_NEAREST, std::string path = "", std::string shading_type = "", glm::vec4 border_color = glm::vec4(0.f), GLenum target = GL_TEXTURE_2D);

    void generateTexture(const GLvoid *data);
    void updateDimensions(const GLsizei width, const GLsizei height);
    void generateMipmaps() const;
    inline void bind() const{glBindTexture(m_target, m_id);}

    //  Getters
    inline GLuint getId()const {return m_id;}
    inline std::string getShadingType()const {return m_shading_type;}
    inline std::string getPath()const {return m_path;}

private:
    GLenum m_target;
    GLuint m_id;
    GLint m_internal_format;
    GLsizei m_width;
    GLsizei m_height;
    GLenum m_format;
    GLenum m_type;
    GLuint m_filter_min;
    GLuint m_filter_max;
    GLuint m_clamp;
    GLfloat m_border_color[4];
    std::string m_shading_type;
    std::string m_path;
};

#endif // TEXTURE_H
