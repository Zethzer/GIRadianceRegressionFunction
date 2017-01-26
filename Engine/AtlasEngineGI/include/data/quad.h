#ifndef QUAD_H
#define QUAD_H

#include "mesh.h"

class Quad : public Mesh
{
public:
    Quad(const GLfloat &size);

    virtual void draw() const;
    virtual void setupBuffers();

private:
    GLfloat m_size;
};

#endif // QUAD_H
