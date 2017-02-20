#include "include/data/geometry/mesh.h"

Mesh::Mesh() :
    m_VAO(0),
    m_VBO(0),
    m_EBO(0),
    m_has_normal_map(GL_FALSE)
{

}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, GLboolean has_normal_map) :
    m_VAO(0),
    m_VBO(0),
    m_EBO(0),
    m_bone_buffer(0),
    m_has_normal_map(has_normal_map)
{
    m_vertices = vertices;
    m_indices = indices;
	
	std::cout << "Indices: ";
	for (int i = 0; i < m_indices.size(); ++i)
		std::cout << m_indices[i] << " ";
	std::cout << std::endl;
	for (int i = 0; i < m_vertices.size(); ++i) {
		std::cout << "Vertex:" << std::endl;
		std::cout << "Position: ";
		std::cout << "{ " << m_vertices[i].Position.x << " " << m_vertices[i].Position.y << " " << m_vertices[i].Position.z << " }" << std::endl;
		std::cout << "Normal: ";
		std::cout << "{ " << m_vertices[i].Normal.x << " " << m_vertices[i].Normal.y << " " << m_vertices[i].Normal.z << " }" << std::endl;
		std::cout << "TexCoords: ";
		std::cout << "{ " << m_vertices[i].TexCoords.x << " " << m_vertices[i].TexCoords.y << " }" << std::endl;
	}

    buildAABB();

    setupBuffers();
}

Mesh::~Mesh()
{

}

void Mesh::draw() const
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setupBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    if(m_has_normal_map)
    {
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
    }

    glBindVertexArray(0);
}

void Mesh::buildAABB()
{
    for(size_t i = 0; i < m_vertices.size(); ++i)
        m_AABB.clipPoint(m_vertices[i].Position);
}
