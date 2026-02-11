#include "propertiesdock.h"
#include "xmlparser.h"
#include "shapes.h"
#include <QGraphicsItem>
#include <QtCore/qnamespace.h>
#include <QtCore/qpoint.h>
#include <QtGui/qbrush.h>
#include <QtGui/qcolor.h>
#include <string>
#include <QPainter>


Rectangle::Rectangle(float x, float y, float w, float h)
    : QGraphicsRectItem(x, y, w, h)
{}


Rectangle::Rectangle(const XMLTag& xml)
    : QGraphicsRectItem()
{
    this->UpdateFromXML(xml);
}

void Rectangle::UpdateFromXML(const XMLTag& xml)
{
    this->setRect(stof(xml.properties.at("x")),
        stof(xml.properties.at("y")),
        stof(xml.properties.at("width")),
        stof(xml.properties.at("height")));
    if(xml.properties.find("rx") != xml.properties.end()) 
    {
        rx = std::stof(xml.properties.at("rx"));
    }
    if(xml.properties.find("ry") != xml.properties.end()) 
    {
        ry = std::stof(xml.properties.at("ry"));
    }
    this->LoadStylesFromXML(this, xml);
    prepareGeometryChange();
    update();
}

XMLTag Rectangle::ToXML() const
{
    XMLTag xml(false);
    xml.name = "rect";
    xml.properties["x"] = std::to_string(rect().topLeft().x());
    xml.properties["y"] = std::to_string(rect().topLeft().y());
    xml.properties["width"] = std::to_string(this->rect().width());
    xml.properties["height"] = std::to_string(this->rect().height());
    xml.properties["rx"] = std::to_string(rx);
    xml.properties["ry"] = std::to_string(ry);
    this->AddStylesToXML(this, xml);
    return xml;
}

void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Must overload to render the rounded corners as rounded rectangle item is not provided by qt
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawRoundedRect(rect(), rx, ry);
    // Render a thick red dotted outline if it is selected
    if(this->isSelected()) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QBrush(Qt::red), 2, Qt::DotLine));
        painter->drawRect(rect());
    }
}

void Rectangle::UpdateShapeOnDraw(QPointF start, QPointF cur)
{
    float x = fmin(start.x(), cur.x());
    float y = fmin(start.y(), cur.y());
    float w = abs(start.x() - cur.x());
    float h = abs(start.y() - cur.y());
    this->setRect(x, y, w, h);
}

PropertiesForm* Rectangle::GetPropertyForm(QWidget* parent)
{
    return new RectPropForm(parent, static_cast<GraphicScene*>(this->scene()), this);
}

