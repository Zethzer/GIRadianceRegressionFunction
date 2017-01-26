#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cstring>

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

struct VertexBoneData;

struct AABB
{
    glm::vec3 V[2];
    
    AABB()
    {
        V[0] = glm::vec3(INFINITY);
        V[1] = glm::vec3(-INFINITY);
    }
    
    void clipPoint(const glm::vec3 &P)
    {
        V[0] = glm::min(P, V[0]);
        V[1] = glm::max(P, V[1]);
    }
};

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    
    Vertex()
    {
        
    }
    
    Vertex(const glm::vec3 &p, const glm::vec2 &tc) :
    Position(p),
    Normal(0.f),
    TexCoords(tc),
    Tangent(0.f),
    Bitangent(0.f)
    {
        
    }
};


//  Used by Kd-Tree
struct Triangle
{
    Vertex* v1;
    Vertex* v2;
    Vertex* v3;
    GLchar side;
    GLuint model;
    GLuint mesh;
    
    Triangle() :
    v1(0),
    v2(0),
    v3(0)
    {
        
    }
    
    Triangle(Vertex *vert1, Vertex *vert2, Vertex *vert3) :
    v1(vert1),
    v2(vert2),
    v3(vert3)
    {
        
    }
};

class Mesh
{
    public:
    Mesh();
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, GLboolean has_normal_map = GL_FALSE);
    virtual ~Mesh();
    
    virtual void draw() const;
    virtual void setupBuffers();

    void computeAABB();
    
    //  Getters
    inline GLuint numIndices() const{return m_indices.size();}
    inline GLuint numVertices() const{return m_vertices.size();}
    inline GLuint getIndex(const GLuint &i) const{return m_indices[i];}
    inline Vertex getVertex(const GLuint &i) const{return m_vertices[i];}
    inline Triangle *getTriangle(const GLuint &i1, const GLuint &i2, const GLuint &i3){return new Triangle(&m_vertices[i1], &m_vertices[i2], &m_vertices[i3]);}
    
    protected:
    GLuint m_VAO, m_VBO, m_EBO, m_bone_buffer;
    
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    
    GLboolean m_has_normal_map;
    
    AABB m_box;
};

#endif // MESH_H
