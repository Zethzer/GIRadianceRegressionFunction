#include "include/render/pipeline.h"
#include "include/data/scene.h"
#include "include/data/quad.h"

#include <queue>

Pipeline::Pipeline(const GLuint &width, const GLuint &height) :
    m_width(width),
    m_height(height)
{

}

Pipeline::~Pipeline()
{
    for(GLuint i = 0; i < m_processes.size(); ++i)
        delete m_processes[i];

    m_processes.clear();
}

void Pipeline::resize(const GLuint &width, const GLuint &height)
{
    m_width = width;
    m_height = height;

    for(GLuint i = 0; i < m_processes.size(); ++i)
        m_processes[i]->resize(m_width, m_height);
}

void Pipeline::process(const Quad &quad, const Scene &scene, const GLfloat &render_time, const GLboolean (&keys)[1024]) const
{
    for(GLuint i = 0; i < m_processes.size(); ++i)
        m_processes[i]->process(quad, scene, render_time, keys);
}

/*
 * Processes are organized like a tree
 * the root will be the last one
 *
 * this method will traverse the tree
 * and build the processes list
 * respecting dependencies between processes
 *
 * a process x previous to a process y
 * will always be before y in the list
 * */
void Pipeline::setLastProcess(RenderProcess *render_process)
{
    /* Current process is at its correct place in the list
     * will change if any process which is "read" after current
     * process is previous to it
     * */
    std::vector<GLboolean> process_valid;

    std::vector<RenderProcess *> processes;
    std::queue<RenderProcess *> processes_queue;

    processes_queue.push(render_process);

    while(!processes_queue.empty())
    {
        RenderProcess *current_process = processes_queue.front();
        processes_queue.pop();

        for(size_t i = 0; i < processes.size(); ++i)
            if(processes[i] == current_process)
                process_valid[i] = GL_FALSE;

        processes.push_back(current_process);
        process_valid.push_back(GL_TRUE);

        for(size_t i = 0; i < current_process->numberPreviousProcesses(); ++i)
            processes_queue.push(current_process->getPreviousProcess(i));
    }

    //  Fill m_processes in the right order
    for(GLuint i = processes.size(); i > 0; --i)
        if(process_valid[i-1])
        {
            m_processes.push_back(processes[i-1]);
            m_processes.back()->init(m_width, m_height);

            if(m_processes.back()->getMenuElement() != 0)
                m_menu_elements.push_back(m_processes.back()->getMenuElement());
        }
}
