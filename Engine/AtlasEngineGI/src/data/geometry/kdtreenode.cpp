#include "include/data/geometry/kdtreenode.h"

KdTreeNode::KdTreeNode()
{

}

KdTreeNode::KdTreeNode(const GLuint &T_size, const std::vector<Triangle *> &T, const AABB &B) :
m_isleaf(true),
m_number_of_triangles(T_size),
m_box(B)
{
    m_triangles = T;
}

KdTreeNode::KdTreeNode(const Plane &plane, KdTreeNode* left, KdTreeNode* right) :
m_isleaf(false),
m_left(left),
m_right(right),
m_plane(plane)
{

}
