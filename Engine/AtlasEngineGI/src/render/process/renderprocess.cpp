#include "include/render/process/renderprocess.h"
#include "include/data/quad.h"

RenderProcess::RenderProcess(const GLuint &num_inputs_textures) :
    m_activated(GL_TRUE),
    m_in_links(0),
    m_menu_element(0)
{
    if(num_inputs_textures > 0)
        m_in_links = new ProcessTextureLink[num_inputs_textures];
}

RenderProcess::~RenderProcess()
{
    //for(GLuint i = 0; i < m_previous_processes.size(); ++i)
    //    delete m_previous_processes[i];

    //m_previous_processes.clear();

    if(m_in_links != 0)
        delete[] m_in_links;
}

void RenderProcess::init(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;

    initMenuElement();
}

/*
 * r1 outputs will be connected to r2 outputs
 *
 * r1_texture_indices contains the indices of
 * the outputs of r1 that will be connected to
 * the inputs of r2 at the indices r2_texture_indices
 * */
GLboolean connectProcesses(RenderProcess *r1, RenderProcess *r2, const std::initializer_list<GLuint> &r1_texture_indices, const std::initializer_list<GLuint> &r2_texture_indices)
{
    if(r1_texture_indices.size() > 0 && r1_texture_indices.size() == r2_texture_indices.size())
    {
        size_t r1_index = r2->numberPreviousProcesses();

        r2->addPreviousProcess(r1);

        for(const GLuint *it1 = r1_texture_indices.begin(), *it2 = r2_texture_indices.begin(); it1 != r1_texture_indices.end(); ++it1, ++it2)
            r2->setInLink(*it1, r1_index, *it2);

        return GL_TRUE;
    }

    return GL_FALSE;
}
