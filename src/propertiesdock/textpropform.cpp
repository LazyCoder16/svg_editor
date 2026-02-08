#include "commands.h"
#include "propertiesdock.h"
#include "shapes.h"
#include "undostack.h"
#include "xmlparser.h"
#include <QGraphicsRectItem>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayoutitem.h>
#include <QtWidgets/qplaintextedit.h>
#include <QtWidgets/qspinbox.h>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <memory>
#include <string>
#include <QWidget>


TextPropForm::TextPropForm(QWidget* parent, GraphicScene* scene, TextShape* shape)
    : PropertiesForm(parent, scene), shape_(shape)
{
    // Initialize
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    this->old_ = shape->ToXML();
    auto font_size = this->GetSpinBox(shape->font().pointSizeF(), false, "pt");
    auto opacity = this->GetSlider(shape->brush());
    auto fill_color_btn = this->GetColorButton(shape->brush().color());
    auto stroke_width_sb = this->GetSpinBox(shape->pen().widthF());
    auto stroke_color_btn = this->GetColorButton(shape->pen().color());
    auto text_edit = new QPlainTextEdit(this);
    if(shape->pen().style() == Qt::NoPen) {
        stroke_width_sb->setValue(0);
    }
    text_edit->setPlainText(shape->text());
    text_edit->setFixedHeight(100);
    text_edit->setFixedWidth(150);

    // Prevent cycle and update
    connect(scene->GetUndoStack(), &UndoStack::StackChanged, this, [=]() {
        this->block_signals = true;
        font_size->setValue(shape->font().pointSizeF());
        opacity->setValue(stoi(shape->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fill_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(shape->brush().color().name()));
        stroke_width_sb->setValue(shape->pen().widthF());
        if(shape->pen().style() == Qt::NoPen) {
            stroke_width_sb->setValue(0);
        }
        stroke_color_btn->setStyleSheet(QString("background-color: %1; border: none;").arg(shape->pen().color().name()));
        this->block_signals = false;
    });
    // Trigger actions on change
    this->ImplSpinBoxChange(shape, font_size, "font-size");
    this->ImplFillColor(shape, opacity, fill_color_btn);
    this->ImplStrokeStyle(shape, stroke_width_sb, stroke_color_btn);
    connect(text_edit, &QPlainTextEdit::textChanged, this, [=]() {
        if(block_signals) return;
        shape->setText(text_edit->toPlainText());
    });
    connect(qApp, &QApplication::focusChanged, this, [=](QWidget* before, QWidget* now) {
        if(now == text_edit) {
            this->old_ = shape->ToXML();
        }
        else if(before == text_edit) {
            SaveCurChanges();
        }
    });
    // Render
    auto text_group_box = new QGroupBox("Text", this);
    auto tlayout = new QFormLayout(text_group_box);
    tlayout->addRow(GetLabel("Font Size"), font_size);
    tlayout->addRow(GetLabel("Text"), text_edit);
    auto styles_group_box = new QGroupBox("Styles", this);
    auto slayout = new QFormLayout(styles_group_box);
    slayout->addRow(GetLabel("Fill opacity"), opacity);
    slayout->addRow("Fill color", fill_color_btn);
    slayout->addRow(GetLabel("Stroke width"), stroke_width_sb);
    slayout->addRow(GetLabel("Stroke color"), stroke_color_btn);
    text_group_box->setLayout(tlayout);
    styles_group_box->setLayout(slayout);
    main_layout->addWidget(text_group_box);
    main_layout->addWidget(styles_group_box);
    main_layout->addStretch();
}

void TextPropForm::SaveCurChanges()
{
    XMLTag cur = shape_->ToXML();
    if(cur.children[0].name != old_.children[0].name) {
        AddXMLAction(shape_, old_, cur);
    }
}


TextPropForm::~TextPropForm()
{
    disconnect(qApp, nullptr, this, nullptr);
}
