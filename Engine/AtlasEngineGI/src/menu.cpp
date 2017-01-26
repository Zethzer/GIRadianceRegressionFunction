#include "menu.h"
#include <QtWidgets>
#include <QApplication>

#include "menuelement.h"

Menu::Menu(QWidget * parent, Qt::WindowFlags f) :
    QDialog(parent, f)
{
    connect(this, SIGNAL(rejected()), this->parent(), SLOT(unPause()));

    QPushButton *resume_button = new QPushButton(tr("Resume"));
    QPushButton *graphics_button = new QPushButton(tr("Graphics"));
    QPushButton *quit_button = new QPushButton(tr("Quit"));

    connect(resume_button, SIGNAL(clicked()), this->parent(), SLOT(unPause()));
    connect(resume_button, &QAbstractButton::clicked, this, &QWidget::close);
    connect(graphics_button, SIGNAL(clicked()), this, SLOT(graphicsMenu()));
    connect(quit_button, SIGNAL(clicked()), qApp, SLOT(quit()));

    m_stacked_widget = new QStackedWidget;

    QVBoxLayout *vertical_layout = new QVBoxLayout;
    vertical_layout->addWidget(resume_button);
    vertical_layout->addWidget(graphics_button);
    vertical_layout->addWidget(quit_button);
    QWidget *menu_widget = new QWidget;
    menu_widget->setLayout(vertical_layout);

    //  Menu widgets
    m_stacked_widget->addWidget(menu_widget);

    QHBoxLayout *horizontal_layout = new QHBoxLayout;
    horizontal_layout->addWidget(m_stacked_widget);

    setLayout(horizontal_layout);
}

void Menu::init()
{

}

void Menu::setGraphicsMenu()
{
    QVBoxLayout *vertical_layout = new QVBoxLayout;

    QPushButton *apply_button = new QPushButton(tr("Apply"));
    QPushButton *discard_button = new QPushButton(tr("Discard"));

    connect(apply_button, SIGNAL(clicked()), this, SLOT(exitGraphicsApply()));
    connect(discard_button, SIGNAL(clicked()), this, SLOT(exitGraphicsDiscard()));

    QHBoxLayout *horizontal_buttons_layout = new QHBoxLayout;
    horizontal_buttons_layout->addWidget(apply_button);
    horizontal_buttons_layout->addWidget(discard_button);

    QFrame *buttons_frame = new QFrame;
    buttons_frame->setLayout(horizontal_buttons_layout);

    for(GLuint i = 0; i < m_graphics_elements.size(); ++i)
        vertical_layout->addWidget(m_graphics_elements[i]);
    vertical_layout->addWidget(buttons_frame);
    QWidget *graphics_widget = new QWidget;
    graphics_widget->setLayout(vertical_layout);

    m_stacked_widget->insertWidget(1, graphics_widget);
}

void Menu::graphicsMenu()
{
    m_stacked_widget->setCurrentIndex(1);
}

void Menu::exitGraphicsDiscard()
{
    m_stacked_widget->setCurrentIndex(0);
}

void Menu::exitGraphicsApply()
{
    m_stacked_widget->setCurrentIndex(0);
}
