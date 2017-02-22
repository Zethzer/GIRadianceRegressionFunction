#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QSurfaceFormat>
#else
//#include <QGLFormat> pour glbindImageTexture à enlever
#endif

#include <string>

#include "ui_mainwindow.h"

namespace Ui {
class AtlasEngine;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
	MainWindow(QWidget *parent = 0, std::string sceneFile = "");
    ~MainWindow();

	void setSceneFile(std::string sceneF) { sceneFile = sceneF; }
	std::string getSceneFile() { return sceneFile; }

private:
    Ui::AtlasEngine *ui;
	std::string sceneFile;
};

#endif // MAINWINDOW_H
