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
    : PropertiesForm(parent, scene), shape(shape)
{
    // Initialize
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    this->old = shape->toXML();
    auto fontSize = this->getSpinBox(shape->font().pointSizeF(), false, "pt");
    auto opacity = this->getSlider(shape->brush());
    auto fillColor = this->getColorButton(shape->brush().color());
    auto strokeWidth = this->getSpinBox(shape->pen().widthF());
    auto strokeColor = this->getColorButton(shape->pen().color());
    auto textEdit = new QPlainTextEdit(this);
    if(shape->pen().style() == Qt::NoPen) {
        strokeWidth->setValue(0);
    }
    textEdit->setPlainText(shape->text());
    textEdit->setFixedHeight(100);
    textEdit->setFixedWidth(150);

    // Prevent cycle and update
    connect(&scene->undoStack, &UndoStack::stackChanged, this, [=]() {
        this->blockSignals = true;
        fontSize->setValue(shape->font().pointSizeF());
        opacity->setValue(stoi(shape->brush().color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16));
        fillColor->setStyleSheet(QString("background-color: %1; border: none;").arg(shape->brush().color().name()));
        strokeWidth->setValue(shape->pen().widthF());
        if(shape->pen().style() == Qt::NoPen) {
            strokeWidth->setValue(0);
        }
        strokeColor->setStyleSheet(QString("background-color: %1; border: none;").arg(shape->pen().color().name()));
        this->blockSignals = false;
    });
    // Trigger actions on change
    this->implSpinBoxChange(shape, fontSize, "font-size");
    this->implFillColor(shape, opacity, fillColor);
    this->implStrokeStyle(shape, strokeWidth, strokeColor);
    connect(textEdit, &QPlainTextEdit::textChanged, this, [=]() {
        if(blockSignals) return;
        shape->setText(textEdit->toPlainText());
    });
    connect(qApp, &QApplication::focusChanged, this, [=](QWidget* before, QWidget* now) {
        if(now == textEdit) {
            this->old = shape->toXML();
        }
        else if(before == textEdit) {
            XMLTag cur = old;
            cur.children[0].name = textEdit->toPlainText().toStdString();
            this->addXMLAction(shape, old, cur);
        }
    });
    // Render
    auto groupBox = new QGroupBox("Text", this);
    auto layout = new QFormLayout(groupBox);
    layout->addRow(getLabel("Font Size"), fontSize);
    layout->addRow(getLabel("Text"), textEdit);
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
    mainLayout->addStretch();
}


TextPropForm::~TextPropForm()
{
    disconnect(qApp, nullptr, this, nullptr);
}
