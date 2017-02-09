#ifndef OPENGLINCLUDES_H
#define OPENGLINCLUDES_H

#include <iostream>

#ifdef __APPLE__
    #include <QGLWidget>
#elif __unix__
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#elif _WIN32
    #include "lib/glew/glew.h"
#endif

#endif // OPENGLINCLUDES_H
