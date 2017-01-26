#ifndef PROCESS_H
#define PROCESS_H

#include "openglincludes.h"
#include "menuelement.h"
#include "texture.h"

#include <initializer_list>

class Quad;
class Framebuffer;
class Scene;

class RenderProcess
{
    struct ProcessTextureLink
    {
        GLuint process_index;
        GLuint texture_index;
    };

public:
    RenderProcess(const GLuint &num_inputs_textures);
    virtual ~RenderProcess();

    virtual void init(const GLuint &width, const GLuint &height);
    virtual void initMenuElement(){}

    inline virtual void resize(const GLuint &width, const GLuint &height){m_width = width; m_height = height;}
    virtual void process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) = 0;

    inline void addPreviousProcess(RenderProcess *render_process){m_previous_processes.push_back(render_process);}

    //  Getters
    inline Texture *getOutTexture(const GLuint &i) const{return m_out_textures[i];}
    inline GLuint getNumOutTexture() const{return m_out_textures.size();}
    inline GLuint width() const{return m_width;}
    inline GLuint height() const{return m_height;}
    inline MenuElement *getMenuElement() const{return m_menu_element;}
    inline size_t numberPreviousProcesses() const{return m_previous_processes.size();}
    inline RenderProcess *getPreviousProcess(const GLuint &i) const{return m_previous_processes[i];}

    //  Setters
    inline virtual void setActivated(const GLboolean &activated){m_activated = activated;}
    inline void setInLink(const GLuint &link_index, const GLuint &render_process_index, const GLuint &tex_index){m_in_links[tex_index].process_index = render_process_index; m_in_links[tex_index].texture_index = link_index;}

protected:
    inline void bindPreviousTexture(const GLuint &i) const{m_previous_processes[m_in_links[i].process_index]->getOutTexture(m_in_links[i].texture_index)->bind();}

    GLuint m_width;
    GLuint m_height;

    size_t m_out_number;

    GLboolean m_activated;

    std::vector<Texture *> m_out_textures;

    ProcessTextureLink *m_in_links;
    std::vector<RenderProcess *> m_previous_processes;

    MenuElement *m_menu_element;
};


/*
 * Connect indices_size textures of 2 processes
 * each r1 out texture at indexes r1_texture_indices
 * will be connected to
 * r2 in textures at r2_texture_indices
 * */
GLboolean connectProcesses(RenderProcess *r1, RenderProcess *r2, const std::initializer_list<GLuint> &r1_texture_indices, const std::initializer_list<GLuint> &r2_texture_indices);

#endif // PROCESS_H
