#ifndef MENUBUTTONSWIDGET_H
#define MENUBUTTONSWIDGET_H

#include <QWidget>

namespace Ui {
class MenuButtonsWidget;
}

class MenuButtonsWidget : public QWidget
{
    Q_OBJECT

public:
    Ui::MenuButtonsWidget *ui;
    explicit MenuButtonsWidget(QWidget *parent = nullptr);
    ~MenuButtonsWidget();
};

#endif // MENUBUTTONSWIDGET_H
