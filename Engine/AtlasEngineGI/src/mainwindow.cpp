#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QThread>

#include "include/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AtlasEngine)
{
    move(0, 0);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
