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


RectPropForm::RectPropForm(QWidget* parent, GraphicScene* scene, Rectangle* rect)
    : PropertiesForm(parent, scene)
{
    // Initialize
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    auto width = this->GetSpinBox(rect->rect().width());
    auto height = this->GetSpinBox(rect->rect().height());
    auto rx = this->GetSpinBox(rect->rx);
    auto ry = this->GetSpinBox(rect->ry);
    auto opacity = this->GetSlider(rect->brush());
    auto fill_color_btn = this->GetColorButton(rect->brush().color());
    auto stroke_width_sb = this->GetSpinBox(rect->pen().widthF());
    auto stroke_color_btn = this->GetColorButton(rect->pen().color());
    // Prevent cycle and update
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        width->setValue(rect->rect().width());
        height->setValue(rect->rect().height());
        rx->setValue(rect->rx);
        ry->setValue(rect->ry);
        opacity->setValue(stoi(rect->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fill_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(rect->brush().color().name()));
        stroke_width_sb->setValue(rect->pen().widthF());
        stroke_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(rect->pen().color().name()));
        this->block_signals = false;
    });
    // Trigger actions on change
    this->ImplSpinBoxChange(rect, width, "width");
    this->ImplSpinBoxChange(rect, height, "height");
    this->ImplSpinBoxChange(rect, rx, "rx");
    this->ImplSpinBoxChange(rect, ry, "ry");
    this->ImplFillColor(rect, opacity, fill_color_btn);
    this->ImplStrokeStyle(rect, stroke_width_sb, stroke_color_btn);
    // Render
    auto geo_group_box = new QGroupBox("Geometry", this);
    auto glayout = new QFormLayout(geo_group_box);
    glayout->addRow(GetLabel("Width"), width);
    glayout->addRow(GetLabel("Height"), height);
    glayout->addRow(GetLabel("Radius X"), rx);
    glayout->addRow(GetLabel("Radius Y"), ry);
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
