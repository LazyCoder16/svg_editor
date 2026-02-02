#include "xmlparser.h"
#include "shapes.h"
#include <QGraphicsItem>
#include <QtCore/qpoint.h>
#include <string>
#include <QPainter>


Rectangle::Rectangle(float x, float y, float w, float h)
    : QGraphicsRectItem(x, y, w, h)
{
    // Use convenient functions Qt provides which will be useful in editting part with mouse events
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}


Rectangle::Rectangle(const XMLTag& xml)
    : QGraphicsRectItem(
        stof(xml.properties.at("x")),
        stof(xml.properties.at("y")),
        stof(xml.properties.at("width")),
        stof(xml.properties.at("height"))
    )
{
    if(xml.properties.find("rx") != xml.properties.end()) 
    {
        rx = std::stof(xml.properties.at("rx"));
    }
    if(xml.properties.find("ry") != xml.properties.end()) 
    {
        ry = std::stof(xml.properties.at("ry"));
    }
    this->loadStylesFromXML(this, xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

XMLTag Rectangle::toXML() const
{
    XMLTag xml(false);
    xml.name = "rect";
    xml.properties["x"] = std::to_string(this->pos().x());
    xml.properties["y"] = std::to_string(this->pos().x());
    xml.properties["width"] = std::to_string(this->rect().width());
    xml.properties["height"] = std::to_string(this->rect().height());
    this->addStylesToXML(this, xml);
    return xml;
}

void Rectangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawRoundedRect(rect(), rx, ry);
}

void Rectangle::updateShapeOnDraw(QPointF start, QPointF cur)
{
    float x = fmin(start.x(), cur.x());
    float y = fmin(start.y(), cur.y());
    float w = abs(start.x() - cur.x());
    float h = abs(start.y() - cur.y());
    this->setRect(x, y, w, h);
}
