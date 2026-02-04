#include "shapes.h"
#include "xmlparser.h"
#include <QGraphicsItem>
#include <QtCore/qpoint.h>
#include <string>



Circle::Circle(float cx, float cy, float rad)
    : QGraphicsEllipseItem(cx-rad, cy-rad, rad*2, rad*2)
{
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

Circle::Circle(const XMLTag& xml)
    : QGraphicsEllipseItem()
{
    updateFromXML(xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

void Circle::updateFromXML(const XMLTag& xml)
{
    float cx = stof(xml.properties.at("cx"));
    float cy = stof(xml.properties.at("cy"));
    float rad = stof(xml.properties.at("r"));
    this->setRect(cx-rad, cy-rad, rad*2, rad*2);
    this->loadStylesFromXML(this, xml);
}

XMLTag Circle::toXML() const
{
    float rad = this->rect().height()/2;
    float cx = rect().topLeft().x() + rad;
    float cy = rect().topLeft().y() + rad;
    XMLTag xml(false);
    xml.name = "circle";
    xml.properties["r"] = std::to_string(rad);
    xml.properties["cx"] = std::to_string(cx);
    xml.properties["cy"] = std::to_string(cy);
    this->addStylesToXML(this, xml);
    return xml;
}

void Circle::updateShapeOnDraw(QPointF start, QPointF cur)
{
    float r = QLineF(start, cur).length();
    this->setRect(start.x()-r, start.y()-r, 2*r, 2*r);
}
