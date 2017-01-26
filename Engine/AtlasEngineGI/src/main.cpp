#include "include/mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOverrideCursor(QCursor(Qt::BlankCursor));

    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(fmt);

    std::string path = argv[0];
#if defined(__WIN32)
    std::replace(path.begin(), path.end(), '\\', '/');
#endif
    MainWindow w;
    w.show();

    return a.exec();
}
