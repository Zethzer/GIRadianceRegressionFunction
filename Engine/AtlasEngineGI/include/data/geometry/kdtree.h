#ifndef KDTREE_H
#define KDTREE_H

/*
 * Implementation of
 *
 * "On building fast kd-Trees for Ray Tracing, and on doing that in O(N log N)"
 *
 * */

#include "kdtreenode.h"

#define EPSILON 0.000001f

#define KT 1    //  Box traversal cost
#define KI 1.5  //  Intersection cost

#define LEFT 0
#define RIGHT 1
#define MIDDLE 2

#define BOTH 0
#define LEFT_ONLY 1
#define RIGHT_ONLY 2

#define END_EVENT 0
#define PLANAR_EVENT 1
#define START_EVENT 2

#define DIMENSION_X 0
#define DIMENSION_Y 1
#define DIMENSION_Z 2

struct Event
{
    GLfloat position;
    GLchar dimension;
    GLchar type;
    Triangle *ID;
};

class KdTree
{
friend class Ray;
friend class Scene;

public:
    KdTree();

    void setRootBox(const AABB& B);
    KdTreeNode* RecBuild(std::vector<Triangle *> &T, AABB &B, GLuint &E_size, std::vector<Event> &E);

    inline void addTriangle(Triangle *t){m_T.push_back(t);}

private:
    GLfloat C(const GLfloat &PL, const GLfloat &PR, const GLuint &NL, const GLuint &NR);
    GLfloat SA(const AABB &b);
    void SAH(Plane& p, AABB &B, const GLuint &NL, const GLuint &NR, const GLuint &NP, GLfloat& CP);
    void createEventList(GLuint &E_size, std::vector<Event> &E);
    void findPlane(const GLuint &N, AABB &B, const GLuint &E_size, const std::vector<Event> &E, Plane &p, GLfloat &C);
    void classifyLeftRightBoth(const GLuint &T_size, std::vector<Triangle *> &T, const GLuint &E_size, std::vector<Event> &E, const Plane &p);
    void splitEvents(const GLuint &E_size, std::vector<Event> &E, Plane &p, GLuint &EL_size, std::vector<Event> &EL, GLuint &ER_size, std::vector<Event> &ER);
    void splitBox(const AABB &B, const Plane &p, AABB &BL, AABB &BR);
    void splitTriangle(const GLuint &T_size, std::vector<Triangle *> &T, std::vector<Triangle *> &TL, std::vector<Triangle *> &TR);
    void generateEvent(const GLchar &k, const AABB &B, Triangle *t, std::vector<Event> &E);
    GLboolean superpose(const Plane &p, const AABB &B);
    void sortEventList(GLuint &E_size, std::vector<Event> &E);
    void quickSort(std::vector<Event> &E, GLuint &p, GLuint &q);
    GLuint partition(std::vector<Event> &E, const GLuint &p, const GLuint &q);
    void boxMinMax(glm::vec3 &min, glm::vec3 &max, glm::vec3 &position);
    void clipBox(AABB &b, AABB &B);
    void boxIntersectTrianglePlane(Triangle &t, Plane &p, AABB &BL, AABB &BR);

    KdTreeNode *m_root;
    std::vector<Triangle *> m_T;
};

#endif // KDTREE_H
