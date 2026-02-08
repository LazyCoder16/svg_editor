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
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    auto opacity = this->GetSlider(path->brush());
    auto fill_color_btn = this->GetColorButton(path->brush().color());
    auto stroke_width_sb = this->GetSpinBox(path->pen().widthF());
    auto stroke_color_btn = this->GetColorButton(path->pen().color());
    // Prevent cycle and update
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        opacity->setValue(stoi(path->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fill_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(path->brush().color().name()));
        stroke_width_sb->setValue(path->pen().widthF());
        stroke_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(path->pen().color().name()));
        this->block_signals = false;
    });
    // Trigger actions on change
    this->ImplFillColor(path, opacity, fill_color_btn);
    this->ImplStrokeStyle(path, stroke_width_sb, stroke_color_btn);
    // Render
    auto style_group_box = new QGroupBox("Styles", this);
    auto slayout = new QFormLayout(style_group_box);
    slayout->addRow(GetLabel("Fill opacity"), opacity);
    slayout->addRow("Fill color", fill_color_btn);
    slayout->addRow(GetLabel("Stroke width"), stroke_width_sb);
    slayout->addRow(GetLabel("Stroke color"), stroke_color_btn);
    style_group_box->setLayout(slayout);
    main_layout->addWidget(style_group_box);
}
