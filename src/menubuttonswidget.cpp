#include "menubuttonswidget.h"
#include "ui_menubuttonswidget.h"
#include <QFileDialog>
#include <QFile>
#include <QDebug>

MenuButtonsWidget::MenuButtonsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuButtonsWidget)
{
    ui->setupUi(this);

    // Setup menu item button tool tips
    ui->pushButton_New->setToolTip("New File");
    ui->pushButton_Copy->setToolTip("Copy");
    ui->pushButton_Cut->setToolTip("Cut");
    ui->pushButton_Open->setToolTip("Open File");
    ui->pushButton_Paste->setToolTip("Paste");
    ui->pushButton_Redo->setToolTip("Redo");
    ui->pushButton_Save->setToolTip("Save");
    ui->pushButton_SaveAs->setToolTip("Save As");
    ui->pushButton_Undo->setToolTip("Undo");
    ui->pushButton_Delete->setToolTip("Delete Selection");
}

MenuButtonsWidget::~MenuButtonsWidget()
{
    delete ui;
}
