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



PropertiesForm::PropertiesForm(QWidget* parent, GraphicScene* scene)
    : QWidget(parent), scene(scene)
{}

void PropertiesForm::addCommand(std::unique_ptr<Command> command)
{
    this->scene->addAction(std::move(command));
}

QDoubleSpinBox* PropertiesForm::getSpinBox(float init_val, bool neg, const std::string& suffix)
{
    auto spinBox = new QDoubleSpinBox(this);
    spinBox->setSuffix(QString::fromStdString(suffix));
    spinBox->setMaximum(100000);
    if(neg) spinBox->setMinimum(-100000);
    spinBox->setValue(init_val);
    return spinBox;
}

QLabel* PropertiesForm::getLabel(const QString& text, int font_size)
{
    QLabel* label = new QLabel(this);
    label->setText(text);
    label->setStyleSheet(QString("font-size: %1pt;").arg(font_size));
    return label;
}

QSlider* PropertiesForm::getSlider(QBrush ival, int from, int to)
{
    int i = stoi(ival.color().name(QColor::HexArgb).toStdString().substr(1, 2), nullptr, 16);
    if(ival == Qt::NoBrush) i = 0;
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(from, to);
    slider->setValue(i);
    return slider;
}

QPushButton* PropertiesForm::getColorButton(QColor icolor)
{
    QPushButton* button = new QPushButton(this);
    button->setToolTip("Pick color");
    button->setText("");
    button->setFixedHeight(24);
    button->setFixedWidth(24);
    button->setStyleSheet(QString("background-color: %1;").arg(icolor.name()));
    return button;
}

void PropertiesForm::addXMLAction(Shape* shape, const XMLTag& old, const XMLTag& cur)
{
    this->scene->addAction(std::make_unique<ShapeXMLCommand>(
        shape, old, cur
    ));
}

void PropertiesForm::implSpinBoxChange(Shape* shape, QDoubleSpinBox* spinBox, const std::string& propName)
{
    connect(spinBox, &QDoubleSpinBox::editingFinished, this, [=]() {
        if(this->blockSignals) return;
        float x = spinBox->value();
        XMLTag old = shape->toXML(), cur = old;
        cur.properties[propName] = std::to_string(x);
        this->addXMLAction(shape, old, cur);
    });
}

void PropertiesForm::implFillColor(Shape* shape, QSlider* slider, QPushButton* button)
{
    connect(slider, &QSlider::sliderPressed, this, [=]() {
        this->startSliderDragState = shape->toXML();
    });
    connect(slider, &QSlider::valueChanged, this, [=](int val) {
        if(blockSignals) return;
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        std::string ophex = QString("%1").arg(val, 2, 16, QChar('0')).toStdString();
        std::string col = "#" + ophex + qshape->brush().color().name().toStdString().substr(1);
        qshape->setBrush(QBrush(QColor::fromString(col)));
    });
    connect(slider, &QSlider::sliderReleased, this, [=]() {
        XMLTag cur = shape->toXML();
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        cur.properties["fill"] = qshape->brush().color().name(QColor::HexArgb).toStdString();
        this->addXMLAction(shape, startSliderDragState, cur);
    });
    connect(button, &QPushButton::clicked, this, [=]() {
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        QColor newColor = QColorDialog::getColor(
            qshape->brush().color(), 
            this, 
            "Pick Color"
        );
        if(newColor.isValid())
        {
            XMLTag old = shape->toXML(), cur = old;
            std::string ophex = QString("%1").arg(slider->value(), 2, 16, QChar('0')).toStdString();
            cur.properties["fill"] = "#" + ophex + newColor.name().toStdString().substr(1);
            this->addXMLAction(shape, old, cur);
        }
    });
}

void PropertiesForm::implStrokeStyle(Shape* shape, QDoubleSpinBox* spinBox, QPushButton* button)
{
    this->implSpinBoxChange(shape, spinBox, "stroke-width");
    connect(button, &QPushButton::clicked, this, [=]() {
        QAbstractGraphicsShapeItem* qshape = dynamic_cast<QAbstractGraphicsShapeItem*>(shape);
        QColor newColor = QColorDialog::getColor(
            qshape->brush().color(), 
            this, 
            "Pick Color"
        );
        if(newColor.isValid())
        {
            XMLTag old = shape->toXML(), cur = old;
            cur.properties["stroke"] = newColor.name().toStdString();
            this->addXMLAction(shape, old, cur);
        }
    });
}
