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
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    auto viewport = scene->GetViewPortRect();
    auto width = this->GetSpinBox(viewport->rect().width());
    auto height = this->GetSpinBox(viewport->rect().height());
    // Prevent cycle
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        width->setValue(viewport->rect().width());
        height->setValue(viewport->rect().height());
        this->block_signals = false;
    });
    // Trigger actions on change
    connect(width, &QDoubleSpinBox::editingFinished, this, [=]() {
        if(this->block_signals) return;
        float w = width->value();
        scene->AddAction(std::make_unique<ViewportChangeCommand>(
            viewport, viewport->rect().width(), viewport->rect().height(), w, viewport->rect().height()
        ));
    });
    connect(height, &QDoubleSpinBox::editingFinished, this, [=]() {
        if(this->block_signals) return;
        float h = height->value();
        scene->AddAction(std::make_unique<ViewportChangeCommand>(
            viewport, viewport->rect().width(), viewport->rect().height(), viewport->rect().width(), h
        ));
    });
    // Render
    auto group_box = new QGroupBox("Viewport", this);
    auto layout = new QFormLayout(group_box);
    layout->addRow(GetLabel("Width"), width);
    layout->addRow(GetLabel("Height"), height);
    group_box->setLayout(layout);
    main_layout->addWidget(group_box);
}
