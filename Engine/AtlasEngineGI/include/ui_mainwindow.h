/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#else
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QWidget>
#endif
#include "atlaswidget.h"

QT_BEGIN_NAMESPACE

class Ui_AtlasEngine
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    AtlasWidget *openGLWidget;

    void setupUi(QMainWindow *AtlasEngine)
    {
        if (AtlasEngine->objectName().isEmpty())
            AtlasEngine->setObjectName(QStringLiteral("AtlasEngine"));
        AtlasEngine->resize(800, 600);
        centralWidget = new QWidget(AtlasEngine);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new AtlasWidget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy);
        openGLWidget->setMinimumSize(QSize(800, 600));
        openGLWidget->setBaseSize(QSize(0, 0));
        openGLWidget->setFocusPolicy(Qt::WheelFocus);

        gridLayout->addWidget(openGLWidget, 0, 0, 1, 1);

        AtlasEngine->setCentralWidget(centralWidget);

        retranslateUi(AtlasEngine);

        QMetaObject::connectSlotsByName(AtlasEngine);
    } // setupUi

    void retranslateUi(QMainWindow *AtlasEngine)
    {
        AtlasEngine->setWindowTitle(QApplication::translate("AtlasEngine", "AtlasEngine", 0));
    } // retranslateUi

};

namespace Ui {
    class AtlasEngine: public Ui_AtlasEngine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
