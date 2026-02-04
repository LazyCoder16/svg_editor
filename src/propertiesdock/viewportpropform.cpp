#include "commands.h"
#include "propertiesdock.h"
#include "undostack.h"
#include <QGraphicsRectItem>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qspinbox.h>
#include <QGroupBox>
#include <memory>


ViewportPropForm::ViewportPropForm(QWidget* parent, GraphicScene* scene)
    : PropertiesForm(parent, scene)
{
    // Initialize
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    auto viewport = scene->viewportRect;
    auto width = this->getSpinBox(viewport->rect().width());
    auto height = this->getSpinBox(viewport->rect().height());
    // Prevent cycle
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        width->setValue(viewport->rect().width());
        height->setValue(viewport->rect().height());
        this->blockSignals = false;
    });
    // Trigger actions on change
    connect(width, &QDoubleSpinBox::editingFinished, this, [=]() {
        if(this->blockSignals) return;
        float w = width->value();
        scene->addAction(std::make_unique<ViewportChangeCommand>(
            viewport, viewport->rect().width(), viewport->rect().height(), w, viewport->rect().height()
        ));
    });
    connect(height, &QDoubleSpinBox::editingFinished, this, [=]() {
        if(this->blockSignals) return;
        float h = height->value();
        scene->addAction(std::make_unique<ViewportChangeCommand>(
            viewport, viewport->rect().width(), viewport->rect().height(), viewport->rect().width(), h
        ));
    });
    // Render
    auto groupBox = new QGroupBox("Viewport", this);
    auto layout = new QFormLayout(groupBox);
    layout->addRow(getLabel("Width"), width);
    layout->addRow(getLabel("Height"), height);
    groupBox->setLayout(layout);
    mainLayout->addWidget(groupBox);
}
