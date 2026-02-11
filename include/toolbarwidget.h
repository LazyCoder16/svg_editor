#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "Enums.h"
#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class ToolBarWidget;
}

class ToolBarWidget : public QWidget
{
    Q_OBJECT

public:
    Ui::ToolBarWidget *ui;
    explicit ToolBarWidget(QWidget *parent = nullptr);
    ~ToolBarWidget();

signals:
    void ToolChanged(ToolType newTool);  // Signal the GraphicsView object of a tool change

private:
    QButtonGroup *button_group_;
};

#endif // TOOLBARWIDGET_H
