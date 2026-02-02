#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>
#include "doccontroller.h"
#include "graphicscene.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void buttonClicked(const std::string& s);
    void setMousePosition(int x, int y);

//private slots:
//    void on_actionNew_triggered();
//    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
    DocController *docController;
    GraphicScene *scene;
};
#endif // MAINWINDOW_H
