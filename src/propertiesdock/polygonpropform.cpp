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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    auto opacity = this->getSlider(polygon->brush());
    auto fillColor = this->getColorButton(polygon->brush().color());
    auto strokeWidth = this->getSpinBox(polygon->pen().widthF());
    auto strokeColor = this->getColorButton(polygon->pen().color());
    auto radiusSlider = new QSlider(Qt::Horizontal, this);
    radiusSlider->setRange(1, polygon->getRadius()*2);
    radiusSlider->setValue(polygon->getRadius());

    // Prevent cycle and update
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        opacity->setValue(stoi(polygon->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fillColor->setStyleSheet(QString("background-color: %1; border: none;").arg(polygon->brush().color().name()));
        strokeWidth->setValue(polygon->pen().widthF());
        strokeColor->setStyleSheet(QString("background-color: %1; border: none;").arg(polygon->pen().color().name()));
        this->blockSignals = false;
    });
    // Trigger actions on change
    this->implFillColor(polygon, opacity, fillColor);
    this->implStrokeStyle(polygon, strokeWidth, strokeColor);
    this->implRadiusSlider(radiusSlider, polygon);
    // Render
    auto groupBox = new QGroupBox("Geometry", this);
    auto layout = new QFormLayout(groupBox);
    layout->addRow(getLabel("Size"), radiusSlider);
    auto groupBox1 = new QGroupBox("Styles", this);
    auto layout1 = new QFormLayout(groupBox1);
    layout1->addRow(getLabel("Fill opacity"), opacity);
    layout1->addRow("Fill color", fillColor);
    layout1->addRow(getLabel("Stroke width"), strokeWidth);
    layout1->addRow(getLabel("Stroke color"), strokeColor);
    groupBox1->setLayout(layout1);
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(groupBox1);
}


void PolygonPropForm::implRadiusSlider(QSlider* slider, Polygon* polygon)
{
    connect(slider, &QSlider::sliderPressed, this, [=]() {
        this->startDragState = polygon->toXML();
    });
    connect(slider, &QSlider::valueChanged, this, [=](int val) {
        polygon->setRadius(val);
    });
    connect(slider, &QSlider::sliderReleased, this, [=]() {
        auto curState = polygon->toXML();
        this->addXMLAction(polygon, startDragState, curState);
    });
}
