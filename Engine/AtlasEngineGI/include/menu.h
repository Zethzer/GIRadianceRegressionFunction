#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QFrame>
#include <QStackedWidget>

class MenuElement;

class Menu : public QDialog
{
    Q_OBJECT
public:
    Menu(QWidget *parent, Qt::WindowFlags f);

    void init();

    inline void setGraphicsMenuElements(const std::vector<MenuElement *> &menu_elements){m_graphics_elements = menu_elements;setGraphicsMenu();}

private slots:
    void setGraphicsMenu();
    void graphicsMenu();
    void exitGraphicsDiscard();
    void exitGraphicsApply();

private:
    QStackedWidget *m_stacked_widget;

    std::vector<MenuElement *> m_graphics_elements;
};

#endif // MENU_H
