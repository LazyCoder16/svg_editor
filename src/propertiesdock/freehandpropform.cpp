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


FreehandPropForm::FreehandPropForm(QWidget* parent, GraphicScene* scene, FreehandPath* path)
    : PropertiesForm(parent, scene)
{
    // Initialize
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    auto opacity = this->getSlider(path->brush());
    auto fillColor = this->getColorButton(path->brush().color());
    auto strokeWidth = this->getSpinBox(path->pen().widthF());
    auto strokeColor = this->getColorButton(path->pen().color());
    // Prevent cycle and update
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        opacity->setValue(stoi(path->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fillColor->setStyleSheet(QString("background-color: %1; border: none;").arg(path->brush().color().name()));
        strokeWidth->setValue(path->pen().widthF());
        strokeColor->setStyleSheet(QString("background-color: %1; border: none;").arg(path->pen().color().name()));
        this->blockSignals = false;
    });
    // Trigger actions on change
    this->implFillColor(path, opacity, fillColor);
    this->implStrokeStyle(path, strokeWidth, strokeColor);
    // Render
    auto groupBox1 = new QGroupBox("Styles", this);
    auto layout1 = new QFormLayout(groupBox1);
    layout1->addRow(getLabel("Fill opacity"), opacity);
    layout1->addRow("Fill color", fillColor);
    layout1->addRow(getLabel("Stroke width"), strokeWidth);
    layout1->addRow(getLabel("Stroke color"), strokeColor);
    groupBox1->setLayout(layout1);
    mainLayout->addWidget(groupBox1);
}
