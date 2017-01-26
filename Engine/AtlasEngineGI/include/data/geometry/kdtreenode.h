#ifndef NODE_H
#define NODE_H

#include "openglincludes.h"

#include <vector>
#include "mesh.h"

struct Plane
{
    GLchar dimension;
    GLfloat position;
    GLchar side;
};

class KdTreeNode
{
public:
    KdTreeNode();
    KdTreeNode(const GLuint &T_size, const std::vector<Triangle *> &T, const AABB &B);
    KdTreeNode(const Plane &plane, KdTreeNode* left, KdTreeNode* right);

    //  Getter
    inline AABB getBox() const{return m_box;}
    inline Plane getPlane() const{return m_plane;}
    inline KdTreeNode *getLeft() const{return m_left;}
    inline KdTreeNode *getRight() const{return m_right;}
    inline GLboolean isLeaf() const{return m_isleaf;}
    inline Triangle *getTriangle(const GLuint &i) const{return m_triangles[i];}
    inline GLboolean numTriangles() const{return m_triangles.size();}

    //  Setter
    inline void setBox(const AABB &box){m_box = box;}

private:
    GLboolean m_isleaf;
    std::vector <Triangle *> m_triangles;
    GLuint m_number_of_triangles;
    KdTreeNode* m_left;
    KdTreeNode* m_right;
    AABB m_box;
    Plane m_plane;
};

#endif // NODE_H
