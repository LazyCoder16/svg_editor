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


CirclePropForm::CirclePropForm(QWidget* parent, GraphicScene* scene, Circle* circle)
    : PropertiesForm(parent, scene)
{
    // Initialize
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    auto radius = this->GetSpinBox(circle->rect().width());
    auto opacity = this->GetSlider(circle->brush());
    auto fill_color_btn = this->GetColorButton(circle->brush().color());
    auto stroke_width_sb = this->GetSpinBox(circle->pen().widthF());
    auto stroke_color_btn = this->GetColorButton(circle->pen().color());
    // Prevent cycle and update
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        radius->setValue(circle->rect().width()/2);
        opacity->setValue(stoi(circle->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fill_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(circle->brush().color().name()));
        stroke_width_sb->setValue(circle->pen().widthF());
        stroke_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(circle->pen().color().name()));
        this->block_signals = false;
    });
    // Trigger actions on change
    this->ImplSpinBoxChange(circle, radius, "r");
    this->ImplFillColor(circle, opacity, fill_color_btn);
    this->ImplStrokeStyle(circle, stroke_width_sb, stroke_color_btn);
    // Render
    auto geo_group_box = new QGroupBox("Geometry", this);
    auto glayout = new QFormLayout(geo_group_box);
    glayout->addRow(GetLabel("Radius"), radius);
    auto styles_group_box = new QGroupBox("Styles", this);
    auto slayout = new QFormLayout(styles_group_box);
    slayout->addRow(GetLabel("Fill opacity"), opacity);
    slayout->addRow("Fill color", fill_color_btn);
    slayout->addRow(GetLabel("Stroke width"), stroke_width_sb);
    slayout->addRow(GetLabel("Stroke color"), stroke_color_btn);
    geo_group_box->setLayout(glayout);
    styles_group_box->setLayout(slayout);
    main_layout->addWidget(geo_group_box);
    main_layout->addWidget(styles_group_box);
}
