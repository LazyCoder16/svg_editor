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
    button_group_ = new QButtonGroup(this);
    button_group_->addButton(ui->pushButton_Circle, (int)ToolType::Circle);
    button_group_->addButton(ui->pushButton_Hexagon, (int)ToolType::Hexagon);
    button_group_->addButton(ui->pushButton_Line, (int)ToolType::Line);
    button_group_->addButton(ui->pushButton_Pen, (int)ToolType::Freehand);
    button_group_->addButton(ui->pushButton_Rectangle, (int)ToolType::Rectangle);
    button_group_->addButton(ui->pushButton_Select, (int)ToolType::Select);
    button_group_->addButton(ui->pushButton_Text, (int)ToolType::Text);

    // Make every button checkable and add exclusive attribute to button group
    for(QAbstractButton *button : button_group_->buttons())
    {
        button->setCheckable(true);
    }
    button_group_->setExclusive(true);

    // Emit signal whever a tool is changed
    connect(button_group_, &QButtonGroup::idToggled, this, [=](int id, bool checked) {
        if(checked) {
            emit ToolChanged((ToolType)id);
        }
    });
    ui->pushButton_Select->setChecked(true);  // Select button is checked true by default
}

ToolBarWidget::~ToolBarWidget()
{
    delete ui;
}
