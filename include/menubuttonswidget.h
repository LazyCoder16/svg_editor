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

signals:
    void buttonClicked(const std::string& s);
    void file_loaded(const QString& filename, const QString& content);

public slots:
    void on_pushButton_New_clicked();
    //void on_pushButton_Save_clicked();
    //void on_pushButton_SaveAs_clicked();
    //void on_pushButton_Open_clicked();
    //void on_pushButton_Cut_clicked();
    void on_pushButton_Copy_clicked();
    //void on_pushButton_Paste_clicked();
    //void on_pushButton_Undo_clicked();
    //void on_pushButton_Redo_clicked();

private slots:
    void on_pushButton_Open_clicked();
};

#endif // MENUBUTTONSWIDGET_H
