#include "commands.h"
#include "graphicscene.h"
#include "propertiesdock.h"
#include "xmlparser.h"
#include <QtCore/qnamespace.h>
#include <QtGui/qbrush.h>
#include <QtGui/qcolor.h>
#include <QtGui/qfont.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qspinbox.h>
#include <QtWidgets/qwidget.h>
#include <memory>
#include <QColorDialog>
#include <string>



PropertiesForm::PropertiesForm(QWidget* parent, GraphicScene* scene)
    : QWidget(parent), scene_(scene)
{}

void PropertiesForm::AddCommand(std::unique_ptr<Command> command)
{
    this->scene_->AddAction(std::move(command));
}

QDoubleSpinBox* PropertiesForm::GetSpinBox(float init_val, bool neg, const std::string& suffix)
{
    auto spin_box = new QDoubleSpinBox(this);
    spin_box->setSuffix(QString::fromStdString(suffix));
    spin_box->setMaximum(100000);
    if(neg) spin_box->setMinimum(-100000);
    spin_box->setValue(init_val);
    return spin_box;
}

QLabel* PropertiesForm::GetLabel(const QString& text, int font_size)
{
    QLabel* label = new QLabel(this);
    label->setText(text);
    label->setStyleSheet(QString("font-size: %1pt;").arg(font_size));
    return label;
}

QSlider* PropertiesForm::GetSlider(QBrush ival, int from, int to)
{
    int i = stoi(ival.color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16);
    if(ival == Qt::NoBrush) i = 0;
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(from, to);
    slider->setValue(i);
    return slider;
}

QPushButton* PropertiesForm::GetColorButton(QColor icolor)
{
    QPushButton* button = new QPushButton(this);
    button->setToolTip("Pick color");
    button->setText("");
    button->setFixedHeight(24);
    button->setFixedWidth(24);
    button->setStyleSheet(QString("background-color: %1;").arg(icolor.name()));
    return button;
}

void PropertiesForm::AddXMLAction(Shape* shape, const XMLTag& old, const XMLTag& cur)
{
    this->scene_->AddAction(std::make_unique<ShapeXMLCommand>(
        shape, old, cur
    ));
}

void PropertiesForm::ImplSpinBoxChange(Shape* shape, QDoubleSpinBox* spinBox, const std::string& propName)
{
    connect(spinBox, &QDoubleSpinBox::editingFinished, this, [=]() {
        if(this->block_signals) return;
        float x = spinBox->value();
        XMLTag old = shape->ToXML(), cur = old;
        cur.properties[propName] = std::to_string(x);
        this->AddXMLAction(shape, old, cur);
    });
}

void PropertiesForm::ImplFillColor(Shape* shape, QSlider* slider, QPushButton* button)
{
    connect(slider, &QSlider::sliderPressed, this, [=]() {
        this->start_slider_drag_state_ = shape->ToXML();
    });
    connect(slider, &QSlider::valueChanged, this, [=](int val) {
        if(block_signals) return;
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        std::string ophex = QString("%1").arg(val, 2, 16, QChar('0')).toStdString();
        std::string col = "#" + ophex + qshape->brush().color().name().toStdString().substr(1);
        qshape->setBrush(QBrush(QColor::fromString(col)));
    });
    connect(slider, &QSlider::sliderReleased, this, [=]() {
        XMLTag cur = shape->ToXML();
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        cur.properties["fill-opacity"] = std::to_string(slider->value() / 255.0);
        this->AddXMLAction(shape, start_slider_drag_state_, cur);
    });
    connect(button, &QPushButton::clicked, this, [=]() {
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        QColor new_color = QColorDialog::getColor(
            qshape->brush().color(), 
            this, 
            "Pick Color"
        );
        if(new_color.isValid())
        {
            XMLTag old = shape->ToXML(), cur = old;
            cur.properties["fill"] = new_color.name().toStdString();
            this->AddXMLAction(shape, old, cur);
        }
    });
}

void PropertiesForm::ImplStrokeStyle(Shape* shape, QDoubleSpinBox* spinBox, QPushButton* button)
{
    this->ImplSpinBoxChange(shape, spinBox, "stroke-width");
    connect(button, &QPushButton::clicked, this, [=]() {
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        QColor new_color = QColorDialog::getColor(
            qshape->brush().color(), 
            this, 
            "Pick Color"
        );
        if(new_color.isValid())
        {
            XMLTag old = shape->ToXML(), cur = old;
            cur.properties["stroke"] = new_color.name().toStdString();
            this->AddXMLAction(shape, old, cur);
        }
    });
}
