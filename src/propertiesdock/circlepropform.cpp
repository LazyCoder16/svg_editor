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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    auto radius = this->getSpinBox(circle->rect().width());
    auto opacity = this->getSlider(circle->brush());
    auto fillColor = this->getColorButton(circle->brush().color());
    auto strokeWidth = this->getSpinBox(circle->pen().widthF());
    auto strokeColor = this->getColorButton(circle->pen().color());
    // Prevent cycle and update
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        radius->setValue(circle->rect().width()/2);
        opacity->setValue(stoi(circle->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fillColor->setStyleSheet(QString("background-color: %1; border: none;").arg(circle->brush().color().name()));
        strokeWidth->setValue(circle->pen().widthF());
        strokeColor->setStyleSheet(QString("background-color: %1; border: none;").arg(circle->pen().color().name()));
        this->blockSignals = false;
    });
    // Trigger actions on change
    this->implSpinBoxChange(circle, radius, "r");
    this->implFillColor(circle, opacity, fillColor);
    this->implStrokeStyle(circle, strokeWidth, strokeColor);
    // Render
    auto groupBox = new QGroupBox("Geometry", this);
    auto layout = new QFormLayout(groupBox);
    layout->addRow(getLabel("Radius"), radius);
    auto groupBox1 = new QGroupBox("Styles", this);
    auto layout1 = new QFormLayout(groupBox1);
    layout1->addRow(getLabel("Fill opacity"), opacity);
    layout1->addRow("Fill color", fillColor);
    layout1->addRow(getLabel("Stroke width"), strokeWidth);
    layout1->addRow(getLabel("Stroke color"), strokeColor);
    groupBox->setLayout(layout);
    groupBox1->setLayout(layout1);
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(groupBox1);
}
