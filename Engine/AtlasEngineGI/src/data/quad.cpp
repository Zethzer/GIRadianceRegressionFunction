#include "include/data/quad.h"

Quad::Quad(const GLfloat &size) :
    Mesh::Mesh(),
    m_size(size)
{
    m_vertices.push_back(Vertex(glm::vec3(-m_size, m_size, 0.f), glm::vec2(0.f, 1.f)));
    m_vertices.push_back(Vertex(glm::vec3(-m_size, -m_size, 0.f), glm::vec2(0.f, 0.f)));
    m_vertices.push_back(Vertex(glm::vec3(m_size, m_size, 0.f), glm::vec2(1.f, 1.f)));
    m_vertices.push_back(Vertex(glm::vec3(m_size, -m_size, 0.f), glm::vec2(1.f, 0.f)));
}

void Quad::draw() const
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Quad::setupBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
}
