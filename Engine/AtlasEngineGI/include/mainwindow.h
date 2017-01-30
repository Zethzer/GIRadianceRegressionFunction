#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#if defines(__APPLE__) || defined(_WIN32)
#include <QSurfaceFormat>
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
