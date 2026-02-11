#include "commands.h"
#include "propertiesdock.h"
#include "shapes.h"
#include "undostack.h"
#include "xmlparser.h"
#include <QGraphicsRectItem>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qspinbox.h>
#include <QGroupBox>
#include <memory>
#include <string>


PolygonPropForm::PolygonPropForm(QWidget* parent, GraphicScene* scene, Polygon* polygon)
    : PropertiesForm(parent, scene)
{
    // Initialize
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    auto opacity = this->GetSlider(polygon->brush());
    auto fill_color_btn = this->GetColorButton(polygon->brush().color());
    auto stroke_width_sb = this->GetSpinBox(polygon->pen().widthF());
    auto stroke_color_btn = this->GetColorButton(polygon->pen().color());
    auto radius_slider = new QSlider(Qt::Horizontal, this);
    radius_slider->setRange(1, polygon->GetRadius()*2);
    radius_slider->setValue(polygon->GetRadius());

    // Prevent cycle and update
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        opacity->setValue(stoi(polygon->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fill_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(polygon->brush().color().name()));
        stroke_width_sb->setValue(polygon->pen().widthF());
        stroke_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(polygon->pen().color().name()));
        this->block_signals = false;
    });
    // Trigger actions on change
    this->ImplFillColor(polygon, opacity, fill_color_btn);
    this->ImplStrokeStyle(polygon, stroke_width_sb, stroke_color_btn);
    this->ImplRadiusSlider(radius_slider, polygon);
    // Render
    auto geo_group_box = new QGroupBox("Geometry", this);
    auto glayout = new QFormLayout(geo_group_box);
    glayout->addRow(GetLabel("Size"), radius_slider);
    auto styles_group_box = new QGroupBox("Styles", this);
    auto slayout = new QFormLayout(styles_group_box);
    slayout->addRow(GetLabel("Fill opacity"), opacity);
    slayout->addRow("Fill color", fill_color_btn);
    slayout->addRow(GetLabel("Stroke width"), stroke_width_sb);
    slayout->addRow(GetLabel("Stroke color"), stroke_color_btn);
    styles_group_box->setLayout(slayout);
    main_layout->addWidget(geo_group_box);
    main_layout->addWidget(styles_group_box);
}


void PolygonPropForm::ImplRadiusSlider(QSlider* slider, Polygon* polygon)
{
    // For re-sizing the polygon
    connect(slider, &QSlider::sliderPressed, this, [=]() {
        this->start_drag_state_ = polygon->ToXML();
    });
    connect(slider, &QSlider::valueChanged, this, [=](int val) {
        polygon->SetRadius(val);
    });
    connect(slider, &QSlider::sliderReleased, this, [=]() {
        auto cur_state = polygon->ToXML();
        this->AddXMLAction(polygon, start_drag_state_, cur_state);
    });
}
