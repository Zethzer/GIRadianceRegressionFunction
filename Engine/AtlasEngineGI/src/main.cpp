#include "include/mainwindow.h"
#include <QApplication>
#if defines(__APPLE__) || defined(_WIN32)
#include <QSurfaceFormat>
#endif
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOverrideCursor(QCursor(Qt::BlankCursor));

#if defines(__APPLE__) || defined(_WIN32)
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(fmt);
#else
    QGLFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);
#endif

    std::string path = argv[0];
#if defined(__WIN32)
    std::replace(path.begin(), path.end(), '\\', '/');
#endif
    MainWindow w;
    w.show();

    return a.exec();
}
