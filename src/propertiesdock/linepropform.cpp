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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    auto strokeWidth = this->getSpinBox(line->pen().widthF());
    auto strokeColor = this->getColorButton(line->pen().color());
    // Prevent cycle and update
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        strokeWidth->setValue(line->pen().widthF());
        strokeColor->setStyleSheet(QString("background-color: %1; border: none;").arg(line->pen().color().name()));
        this->blockSignals = false;
    });
    // Trigger actions on change
    this->implStrokeStyle(line, strokeWidth, strokeColor);
    // Render
    auto groupBox1 = new QGroupBox("Styles", this);
    auto layout1 = new QFormLayout(groupBox1);
    layout1->addRow(getLabel("Stroke width"), strokeWidth);
    layout1->addRow(getLabel("Stroke color"), strokeColor);
    groupBox1->setLayout(layout1);
    mainLayout->addWidget(groupBox1);
}
