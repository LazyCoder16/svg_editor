#include "commands.h"
#include "propertiesdock.h"
#include "shapes.h"
#include "undostack.h"
#include "xmlparser.h"
#include <QGraphicsRectItem>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qspinbox.h>
#include <QGroupBox>
#include <memory>
#include <string>


LinePropForm::LinePropForm(QWidget* parent, GraphicScene* scene, Line* line)
    : PropertiesForm(parent, scene)
{
    // Initialize
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    auto stroke_width_sb = this->GetSpinBox(line->pen().widthF());
    auto stroke_color_btn = this->GetColorButton(line->pen().color());
    // Prevent cycle and update
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        stroke_width_sb->setValue(line->pen().widthF());
        stroke_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(line->pen().color().name()));
        this->block_signals = false;
    });
    // Trigger actions on change
    this->ImplStrokeStyle(line, stroke_width_sb, stroke_color_btn);
    // Render
    auto styles_group_box = new QGroupBox("Styles", this);
    auto slayout = new QFormLayout(styles_group_box);
    slayout->addRow(GetLabel("Stroke width"), stroke_width_sb);
    slayout->addRow(GetLabel("Stroke color"), stroke_color_btn);
    styles_group_box->setLayout(slayout);
    main_layout->addWidget(styles_group_box);
}
