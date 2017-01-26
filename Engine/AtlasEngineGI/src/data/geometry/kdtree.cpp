#include "include/data/geometry/kdtree.h"

KdTree::KdTree()
{

}

void KdTree::setRootBox(const AABB &B)
{
    m_root->setBox(B);
}

/*
 * Main recursive function
 * */
KdTreeNode* KdTree::RecBuild(std::vector<Triangle *> &T, AABB &B, GLuint &E_size, std::vector<Event> &E)
{
    Plane p;
    GLfloat C;
    GLuint T_size = T.size();

    //  Prevent crashes
    if(T_size < 3)
    {
        //  Leaf
        KdTreeNode *n = new KdTreeNode(T_size, T, B);
        return n;
    }

    //  Compute p
    findPlane(T_size, B, E_size, E, p, C);


    //  End of recursion?
    if(C > T_size * KI || superpose(p, B))
    {
        //  Leaf
        KdTreeNode *n = new KdTreeNode(T_size, T, B);
        return n;
    }

    //  Else, build children

    //  Triangles classification by sides
    classifyLeftRightBoth(T_size, T, E_size, E, p);

    //  Children event lists creation
    std::vector<Event> EL, ER;
    GLuint EL_size, ER_size;
    splitEvents(E_size, E, p, EL_size, EL, ER_size, ER);

    //  Adding children triangles
    std::vector<Triangle *> TL, TR;
    splitTriangle(T_size, T, TL, TR);

    //  Creating children boxes
    AABB BL, BR;
    splitBox(B, p, BL, BR);

    //  Children recursion
    return new KdTreeNode(p, RecBuild(TL, BL, EL_size, EL), RecBuild(TR, BR, ER_size, ER));
}

/*
 *  Returns node building cost
 * */
GLfloat KdTree::C(const GLfloat &PL, const GLfloat &PR, const GLuint &NL,  const GLuint &NR)
{
    GLfloat cost = KT + KI * (PL * NL + PR * NR);

    if(NL == 0 || NR == 0)
        cost *= 0.8;

    return cost;
}

/*
 * Surface Area
 * Returns box surface (actually, half of the surface)
 * */
GLfloat KdTree::SA(const AABB &b)
{
    GLfloat dx = glm::distance(b.V[1].x, b.V[0].x),
            dy = glm::distance(b.V[1].y, b.V[0].y),
            dz = glm::distance(b.V[1].z, b.V[0].z);

    return dy * (dx + dz) + dx * dz;
}

/*
 *  Compute Surface Area Heuristic
 * */
void KdTree::SAH(Plane& p, AABB &B, const GLuint &NL, const GLuint &NR, const GLuint &NP, GLfloat& CP)
{
    AABB BL, BR;

    splitBox(B, p, BL, BR);

    GLfloat inv_SAB = 1.f / SA(B),
            PL = SA(BL) * inv_SAB,
            PR = SA(BR) * inv_SAB,
            CPL = C(PL, PR, NL + NP, NR),
            CPR = C(PL, PR, NL, NR + NP);

    if(CPL < CPR)
    {
        CP = CPL;
        p.side = LEFT;
    }
    else
    {
        CP = CPR;
        p.side = RIGHT;
    }
}

/*
 * Creation of the root event list
 * */
void KdTree::createEventList(GLuint &E_size, std::vector<Event> &E)
{
    GLuint T_size = m_T.size();

    for(GLuint i = 0; i < T_size; ++i)
    {
        AABB b;
        b.V[0] = m_T[i]->v1->Position,
        b.V[1] = b.V[0];
        boxMinMax(b.V[0], b.V[1], m_T[i]->v2->Position);
        boxMinMax(b.V[0], b.V[1], m_T[i]->v3->Position);

        //  Adding events in E for each dimension
        generateEvent(DIMENSION_X, b, m_T[i], E);
        generateEvent(DIMENSION_Y, b, m_T[i], E);
        generateEvent(DIMENSION_Z, b, m_T[i], E);
    }
    E_size = E.size();
    sortEventList(E_size, E);
}

/*
 *  Find cutting plane p
 * */
void KdTree::findPlane(const GLuint &N, AABB &B, const GLuint &E_size, const std::vector<Event> &E, Plane &p, GLfloat &C)
{
    C = INFINITY;

    //  Every triangle is on the right for each k
    GLuint  NL[3] = {0, 0, 0},
            NP[3] = {0, 0, 0},
            NR[3] = {N, N, N};

    //  Iterate over every candidate plane
    for(GLuint i = 0; i < E_size;)
    {
        Plane p_tmp;
        p_tmp.dimension = E[i].dimension;
        p_tmp.position = E[i].position;

        //  Event numbers
        GLuint  p_start = 0,
                p_planar = 0,
                p_end = 0;

        //  Count event number of each type
        while(i < E_size && E[i].dimension == p_tmp.dimension && E[i].position == p_tmp.position && E[i].type == END_EVENT)
        {
            ++p_end;
            ++i;
        }
        while(i < E_size && E[i].dimension == p_tmp.dimension && E[i].position == p_tmp.position && E[i].type == PLANAR_EVENT)
        {
            ++p_planar;
            ++i;
        }
        while(i < E_size && E[i].dimension == p_tmp.dimension && E[i].position == p_tmp.position && E[i].type == START_EVENT)
        {
            ++p_start;
            ++i;
        }
        GLuint k = p_tmp.dimension;

        NP[k] = p_planar;
        NR[k] -= p_planar;
        NR[k] -= p_end;

        GLfloat CP;
        SAH(p_tmp, B, NL[k], NR[k], NP[k], CP);

        //  Find minimal cost
        if(CP < C)
        {
            C = CP;
            p = p_tmp;
        }
        NL[k] += p_start;
        NL[k] += p_planar;
        NP[k] = 0;
    }
}

/*
 * Set triangle sides to LEFT, RIGHT or BOTH
 * according to their position compared to the plane
 * */
void KdTree::classifyLeftRightBoth(const GLuint &T_size, std::vector<Triangle *> &T, const GLuint &E_size, std::vector<Event> &E, const Plane &p)
{
    //  Init triangles to BOTH
    for(GLuint i = 0; i < T_size; ++i)
        T[i]->side = BOTH;

    for(GLuint i = 0; i < E_size; ++i)
    {
        if(E[i].type == END_EVENT && E[i].dimension == p.dimension && E[i].position <= p.position)
            E[i].ID->side = LEFT_ONLY;
        else if(E[i].type == START_EVENT && E[i].dimension == p.dimension && E[i].position >= p.position)
            E[i].ID->side = RIGHT_ONLY;
        else if(E[i].type == PLANAR_EVENT && E[i].dimension == p.dimension)
        {
            if(E[i].position < p.position || (E[i].position == p.position && p.side == LEFT))
                E[i].ID->side = LEFT_ONLY;
            if(E[i].position > p.position || (E[i].position == p.position && p.side == RIGHT))
                E[i].ID->side = RIGHT_ONLY;
        }
    }
}


/*
 *  Set children event sides
 * */
void KdTree::splitEvents(const GLuint &E_size, std::vector<Event> &E, Plane &p, GLuint &EL_size, std::vector<Event> &EL, GLuint &ER_size, std::vector<Event> &ER)
{
    std::vector<Event>  ELO,
                        ERO,
                        EBL,
                        EBR;

    for(GLuint i = 0; i < E_size; ++i)
    {
        GLuint side = E[i].ID->side;

        if(side == LEFT_ONLY)
            ELO.push_back(E[i]);
        else if(side == RIGHT_ONLY)
            ERO.push_back(E[i]);
        else
        {
            Triangle *t = E[i].ID;
            AABB BL,
                BR;

            AABB b;
            b.V[0] = E[i].ID->v1->Position,
            b.V[1] = b.V[0];
            boxMinMax(b.V[0], b.V[1], E[i].ID->v2->Position);
            boxMinMax(b.V[0], b.V[1], E[i].ID->v3->Position);

            //  Creation of left and right boxes separated by plane
            splitBox(b, p, BL, BR);

            generateEvent(E[i].dimension, BL, t, EBL);
            generateEvent(E[i].dimension, BR, t, EBR);
        }
    }
    E.clear();

    GLuint  ELO_size = ELO.size(),
            EBL_size = EBL.size(),
            ERO_size = ERO.size(),
            EBR_size = EBR.size();

    if(ELO_size == 0)
    {
        sortEventList(EBL_size, EBL);
        EL = EBL;
    }
    else if(EBL_size == 0)
        EL = ELO;
    else
    {
        sortEventList(EBL_size, EBL);

        //  Fill EL
        for(GLuint i = 0, j = 0; i < ELO_size || j < EBL_size;)
        {
            if(i < ELO_size)
            {
                if(j < EBL_size)
                {
                    if((ELO[i].position < EBL[j].position) || (ELO[i].position == EBL[j].position && ELO[i].type < EBL[j].type))
                        EL.push_back(ELO[i++]);
                    else
                        EL.push_back(EBL[j++]);
                }
                else
                    EL.push_back(ELO[i++]);
            }
            else
                EL.push_back(EBL[j++]);
        }
    }

    if(ERO_size == 0)
    {
        sortEventList(EBR_size, EBR);
        ER = EBR;
    }
    else if(EBR_size == 0)
        ER = ERO;
    else
    {
        sortEventList(EBR_size, EBR);

        //  Fill ER
        for(GLuint i = 0, j = 0; i < ERO_size || j < EBR_size;)
        {
            if(i < ERO_size)
            {
                if(j < EBR_size)
                {
                    if((ERO[i].position < EBR[j].position) || (ERO[i].position == EBR[j].position && ERO[i].type < EBR[j].type))
                        ER.push_back(ERO[i++]);
                    else
                        ER.push_back(EBR[j++]);
                }
                else
                    ER.push_back(ERO[i++]);
            }
            else
                ER.push_back(EBR[j++]);
        }
    }
    ER_size = ER.size();
    EL_size = EL.size();
}

void KdTree::splitBox(const AABB &B, const Plane &p, AABB &BL, AABB &BR)
{
    BL = B;
    BR = B;

    if(p.dimension == DIMENSION_X)
    {
        BL.V[1].x = p.position;
        BR.V[0].x = BL.V[1].x;
    }
    else if(p.dimension == DIMENSION_Y)
    {
        BL.V[1].y = p.position;
        BR.V[0].y = BL.V[1].y;
    }
    else
    {
        BL.V[1].z = p.position;
        BR.V[0].z = BL.V[1].z;
    }
}

/*
 * Set children triangle lists
 * and boxes
 * */
void KdTree::splitTriangle(const GLuint &T_size, std::vector<Triangle *> &T, std::vector<Triangle *> &TL, std::vector<Triangle *> &TR)
{
    //  Split Triangles
    for(GLuint i = 0; i < T_size; ++i)
    {
        if(T[i]->side == LEFT_ONLY)
            TL.push_back(T[i]);
        else if(T[i]->side == RIGHT_ONLY)
            TR.push_back(T[i]);
        else
        {
            //  Both
            TL.push_back(T[i]);
            TR.push_back(T[i]);
        }
    }
    T.clear();
}

/*
 *  UTILITIES
 * */

/*
 *  Generate 1 (or 2) events from B and k, and push them in E
 * */
void KdTree::generateEvent(const GLchar &k, const AABB &B, Triangle *t, std::vector<Event> &E)
{
    Event e;
    e.ID = t;
    e.dimension = k;

    if(k == DIMENSION_X)
    {
        if(B.V[0].x == B.V[1].x)
        {
            e.type = PLANAR_EVENT;
            e.position = B.V[0].x;
        }
        else
        {
            e.type = END_EVENT;
            e.position = B.V[1].x;
            E.push_back(e);

            e.type = START_EVENT;
            e.position = B.V[0].x;
        }
    }
    else if(k == DIMENSION_Y)
    {
        if(B.V[0].y == B.V[1].y)
        {
            e.type = PLANAR_EVENT;
            e.position = B.V[0].y;
        }
        else
        {
            e.type = END_EVENT;
            e.position = B.V[1].y;
            E.push_back(e);

            e.type = START_EVENT;
            e.position = B.V[0].y;
        }
    }
    else
    {
        if(B.V[0].z == B.V[1].z)
        {
            e.type = PLANAR_EVENT;
            e.position = B.V[0].z;
        }
        else
        {
            e.type = END_EVENT;
            e.position = B.V[1].z;
            E.push_back(e);

            e.type = START_EVENT;
            e.position = B.V[0].z;
        }
    }
    E.push_back(e);
}

/*
 * Returns true if plane is equal to a plane of the box
 * */
GLboolean KdTree::superpose(const Plane &p, const AABB &B)
{
    if(p.dimension == DIMENSION_X)
    {
        if(p.position == B.V[0].x || p.position == B.V[1].x)
            return true;
    }
    else if(p.dimension == DIMENSION_Y)
    {
        if(p.position == B.V[0].y || p.position == B.V[1].y)
            return true;
    }
    else
    {
        if(p.position == B.V[0].z || p.position == B.V[1].z)
            return true;
    }
    return false;
}

/*
 *  QuickSort of event list E
 * */
void KdTree::sortEventList(GLuint &E_size, std::vector<Event> &E)
{
    GLuint zero = 0;
    quickSort(E, zero, E_size);
}
void KdTree::quickSort(std::vector<Event> &E, GLuint &p,GLuint &q)
{
    GLuint r;
    if(p < q)
    {
        r = partition(E, p, q);
        quickSort(E, p, r);
        GLuint rp = r + 1;
        quickSort(E, rp, q);
    }
}
GLuint KdTree::partition(std::vector<Event> &E, const GLuint &p, const GLuint &q)
{
    Event e = E[p];
    GLuint i = p;
    GLuint j;

    for(j = p + 1; j < q; ++j)
    {
        if((E[j].position < e.position) || (E[j].position == e.position && E[j].type < e.type))
        {
            i = i + 1;
            Event tmp = E[j];
            E[j] = E[i];
            E[i] = tmp;
        }
    }

    Event tmp = E[p];
    E[p] = E[i];
    E[i] = tmp;
    return i;
}


/*
 *  Set min and max according to position
 * */
void KdTree::boxMinMax(glm::vec3& min, glm::vec3& max, glm::vec3& position)
{
    if(min.x > position.x)
        min.x = position.x;
    if(min.y > position.y)
        min.y = position.y;
    if(min.z > position.z)
        min.z = position.z;

    if(max.x < position.x)
        max.x = position.x;
    if(max.y < position.y)
        max.y = position.y;
    if(max.z < position.z)
        max.z = position.z;
}

/*
 *  Clip triangle box b inside box B
 * */
void KdTree::clipBox(AABB &b, AABB &B)
{
    if(B.V[0].x > b.V[0].x)
        b.V[0].x = B.V[0].x;
    if(B.V[1].x < b.V[1].x)
        b.V[1].x = B.V[1].x;

    if(B.V[0].y > b.V[0].y)
        b.V[0].y = B.V[0].y;
    if(B.V[1].y < b.V[1].y)
        b.V[1].y = B.V[1].y;

    if(B.V[0].z > b.V[0].z)
        b.V[0].z = B.V[0].z;
    if(B.V[1].z < b.V[1].z)
        b.V[1].z = B.V[1].z;
}

/*
 *  Compute left and right boxes of a triangle cut by a plane
 * */
void KdTree::boxIntersectTrianglePlane(Triangle& t, Plane& p, AABB& BL, AABB& BR)
{
    glm::vec3   X,              //  Point that belongs to the plane
                N,              //  Normal
                P0,             //  Left
                P1,             //  Middle
                P2,             //  Right
                intersection1,
                intersection2,
                min,
                max;
    GLfloat c,
            d0,
            d1;

    GLchar cote_milieu;

    P0 = t.v1->Position;
    P1 = P0;
    P2 = P0;

    //  Define points according to the dimension of the plane

    //  Dimension X
    if(p.dimension == DIMENSION_X)
    {
        X = glm::vec3(p.position, 0, 0);
        N = glm::vec3(-1, 0, 0);

        if(t.v2->Position.x < P0.x)
            P0.x = t.v2->Position.x;
        else
            P2.x = t.v2->Position.x;

        if(t.v3->Position.x < P0.x)
        {
            P1.x = P0.x;
            P0.x = t.v3->Position.x;
        }
        else if(t.v3->Position.x > P2.x)
        {
            P1.x = P2.x;
            P2.x = t.v3->Position.x;
        }
        else
            P1.x = t.v3->Position.x;

        //  Compute intersections
        c = glm::dot(N, X);
        d0 = glm::dot(N, P0) - c;
        d1 = glm::dot(N, P1) - c;

        intersection1 = P0 + (d0 * (1.f / (d0 - d1))) * (P1 - P0);

        d1 = glm::dot(N, P2) - c;

        intersection2 = P0 + (d0 * (1.f / (d0 - d1))) * (P2 - P0);

        if(P1.x < p.position)
            cote_milieu = LEFT;
        else if(P1.x > p.position)
            cote_milieu = RIGHT;
        else
            cote_milieu = MIDDLE;
    }
    //  Dimension Y
    else if(p.dimension == DIMENSION_Y)
    {
        X = glm::vec3(0, p.position, 0);
        N = glm::vec3(0, -1, 0);

        if(t.v2->Position.y < P0.y)
            P0.y = t.v2->Position.y;
        else
            P2.y = t.v2->Position.y;

        if(t.v3->Position.y < P0.y)
        {
            P1.y = P0.y;
            P0.y = t.v3->Position.y;
        }
        else if(t.v3->Position.y > P2.y)
        {
            P1.y = P2.y;
            P2.y = t.v3->Position.y;
        }
        else
            P1.y = t.v3->Position.y;

        //  Compute intersection
        c = glm::dot(N, X);
        d0 = glm::dot(N, P0) - c;
        d1 = glm::dot(N, P1) - c;

        intersection1 = P0 + (d0 * (1.f / (d0 - d1))) * (P1 - P0);

        d1 = glm::dot(N, P2) - c;

        intersection2 = P0 + (d0 * (1.f / (d0 - d1))) * (P2 - P0);

        if(P1.y < p.position)
            cote_milieu = LEFT;
        else if(P1.y > p.position)
            cote_milieu = RIGHT;
        else
            cote_milieu = MIDDLE;
    }
    //  Dimension Z
    else
    {
        X = glm::vec3(0, 0, p.position);
        N = glm::vec3(0, 0, -1);

        if(t.v2->Position.z < P0.z)
            P0.z = t.v2->Position.z;
        else
            P2.z = t.v2->Position.z;

        if(t.v3->Position.z < P0.z)
        {
            P1.z = P0.z;
            P0.z = t.v3->Position.z;
        }
        else if(t.v3->Position.z > P2.z)
        {
            P1.z = P2.z;
            P2.z = t.v3->Position.z;
        }
        else
            P1.z = t.v3->Position.z;

        //  Compute intersections
        c = glm::dot(N, X);
        d0 = glm::dot(N, P0) - c;
        d1 = glm::dot(N, P1) - c;

        intersection1 = P0 + (d0 * (1.f / (d0 - d1))) * (P1 - P0);

        d1 = glm::dot(N, P2) - c;

        intersection2 = P0 + (d0 * (1.f / (d0 - d1))) * (P2 - P0);

        if(P1.z < p.position)
            cote_milieu = LEFT;
        else if(P1.z > p.position)
            cote_milieu = RIGHT;
        else
            cote_milieu = MIDDLE;
    }

    //  Compute boxes
    if(cote_milieu == LEFT)
    {
        //  Left
        min = P0;
        max = min;
        boxMinMax(min, max, P1);
        boxMinMax(min, max, intersection1);
        boxMinMax(min, max, intersection2);
        BL.V[0] = min;
        BL.V[1] = max;

        //  Right
        min = intersection1;
        max = min;
        boxMinMax(min, max, P2);
        boxMinMax(min, max, intersection2);
        BR.V[0] = min;
        BR.V[1] = max;
    }
    else if(cote_milieu == RIGHT)
    {
        //  Left
        min = P0;
        max = min;
        boxMinMax(min, max, intersection1);
        boxMinMax(min, max, intersection2);
        BL.V[0] = min;
        BL.V[1] = max;

        //  Right
        min = intersection1;
        max = min;
        boxMinMax(min, max, P1);
        boxMinMax(min, max, P2);
        boxMinMax(min, max, intersection2);
        BR.V[0] = min;
        BR.V[1] = max;
    }
    else
    {
        //  Left
        min = P1;
        max = min;
        boxMinMax(min, max, P0);
        boxMinMax(min, max, intersection1);
        boxMinMax(min, max, intersection2);
        BL.V[0] = min;
        BL.V[1] = max;

        //  Right
        min = P1;
        max = min;
        boxMinMax(min, max, P2);
        boxMinMax(min, max, intersection1);
        boxMinMax(min, max, intersection2);
        BR.V[0] = min;
        BR.V[1] = max;
    }
}
