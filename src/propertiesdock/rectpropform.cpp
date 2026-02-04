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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    auto width = this->getSpinBox(rect->rect().width());
    auto height = this->getSpinBox(rect->rect().height());
    auto rx = this->getSpinBox(rect->rx);
    auto ry = this->getSpinBox(rect->ry);
    auto opacity = this->getSlider(rect->brush());
    auto fillColor = this->getColorButton(rect->brush().color());
    auto strokeWidth = this->getSpinBox(rect->pen().widthF());
    auto strokeColor = this->getColorButton(rect->pen().color());
    // Prevent cycle and update
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        width->setValue(rect->rect().width());
        height->setValue(rect->rect().height());
        rx->setValue(rect->rx);
        ry->setValue(rect->ry);
        opacity->setValue(stoi(rect->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fillColor->setStyleSheet(QString("background-color: %1; border: none;").arg(rect->brush().color().name()));
        strokeWidth->setValue(rect->pen().widthF());
        strokeColor->setStyleSheet(QString("background-color: %1; border: none;").arg(rect->pen().color().name()));
        this->blockSignals = false;
    });
    // Trigger actions on change
    this->implSpinBoxChange(rect, width, "width");
    this->implSpinBoxChange(rect, height, "height");
    this->implSpinBoxChange(rect, rx, "rx");
    this->implSpinBoxChange(rect, ry, "ry");
    this->implFillColor(rect, opacity, fillColor);
    this->implStrokeStyle(rect, strokeWidth, strokeColor);
    // Render
    auto groupBox = new QGroupBox("Geometry", this);
    auto layout = new QFormLayout(groupBox);
    layout->addRow(getLabel("Width"), width);
    layout->addRow(getLabel("Height"), height);
    layout->addRow(getLabel("Radius X"), rx);
    layout->addRow(getLabel("Radius Y"), ry);
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
