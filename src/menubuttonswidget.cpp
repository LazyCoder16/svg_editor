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

void MenuButtonsWidget::on_pushButton_New_clicked()
{
    emit buttonClicked("New");
}

void MenuButtonsWidget::on_pushButton_Copy_clicked()
{
    emit buttonClicked("Copy");
}

void MenuButtonsWidget::on_pushButton_Open_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open File"), QString(), tr("SVG File (*.svg)"));
    if(!filename.isNull()) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open file:" << filename;
            return;
        }
        qDebug() << filename.toStdString() << "\n";
        QTextStream in(&file);
        emit file_loaded(filename, in.readAll());
        file.close();
    }
}

