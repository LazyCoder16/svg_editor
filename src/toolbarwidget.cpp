#include "toolbarwidget.h"
#include "ui_toolbarwidget.h"
#include "Enums.h"
#include <QtWidgets/qbuttongroup.h>

ToolBarWidget::ToolBarWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ToolBarWidget)
{
    ui->setupUi(this);

    // Set Tooltips
    ui->pushButton_Circle->setToolTip("Circle Tool");
    ui->pushButton_Hexagon->setToolTip("Hexagon Tool");
    ui->pushButton_Line->setToolTip("Line Tool");
    ui->pushButton_Pen->setToolTip("Freehand Pen Tool");
    ui->pushButton_Rectangle->setToolTip("Recangle Tool");
    ui->pushButton_Select->setToolTip("Selection Tool");
    ui->pushButton_Text->setToolTip("Text Tool");

    // Initialize button group
    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_Circle, (int)ToolType::Circle);
    buttonGroup->addButton(ui->pushButton_Hexagon, (int)ToolType::Hexagon);
    buttonGroup->addButton(ui->pushButton_Line, (int)ToolType::Line);
    buttonGroup->addButton(ui->pushButton_Pen, (int)ToolType::Freehand);
    buttonGroup->addButton(ui->pushButton_Rectangle, (int)ToolType::Rectangle);
    buttonGroup->addButton(ui->pushButton_Select, (int)ToolType::Select);
    buttonGroup->addButton(ui->pushButton_Text, (int)ToolType::Text);

    // Make every button checkable and add exclusive attribute to button group
    for(QAbstractButton *button : buttonGroup->buttons())
    {
        button->setCheckable(true);
    }
    buttonGroup->setExclusive(true);

    connect(buttonGroup, &QButtonGroup::idToggled, this, [=](int id, bool checked) {
        if(checked) {
            emit toolChanged((ToolType)id);
        }
    });
    ui->pushButton_Select->setChecked(true);
}

ToolBarWidget::~ToolBarWidget()
{
    delete ui;
}
