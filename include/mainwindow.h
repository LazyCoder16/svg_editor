#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>
#include <QCloseEvent>
#include <QtGui/qevent.h>
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
    void SetMousePosition(int x, int y);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;
    DocController *doc_controller_;
    GraphicScene *scene_;
};
#endif // MAINWINDOW_H
