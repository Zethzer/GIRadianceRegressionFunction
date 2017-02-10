#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QSurfaceFormat>
#else
#include <QGLFormat>
#endif

#include "ui_mainwindow.h"

namespace Ui {
class AtlasEngine;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::AtlasEngine *ui;
};

#endif // MAINWINDOW_H
