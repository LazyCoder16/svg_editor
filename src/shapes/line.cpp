#include "xmlparser.h"
#include "shapes.h"
#include <QGraphicsItem>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <string>


Line::Line(float x1, float y1, float x2, float y2)
    : QGraphicsPolygonItem(QPolygonF(QList<QPointF> {
        QPointF(x1, y1), QPointF(x2, y2)
    }))
{
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}


Line::Line(const XMLTag& xml)
    : QGraphicsPolygonItem()
{
    this->updateFromXML(xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}


void Line::updateFromXML(const XMLTag& xml)
{
    this->setPolygon(QPolygonF(QList<QPointF> {
        QPointF(std::stof(xml.properties.at("x1")), std::stof(xml.properties.at("y1"))), 
        QPointF(std::stof(xml.properties.at("x2")), std::stof(xml.properties.at("y2")))
    }));
    this->loadStylesFromXML(this, xml);
}


XMLTag Line::toXML() const
{
    XMLTag xml(false);
    xml.name = "line";
    QList<QPointF> points = this->polygon().toList();
    QPointF p1 = points.first();
    QPointF p2 = points.last();
    xml.properties["x1"] = std::to_string(p1.x());
    xml.properties["y1"] = std::to_string(p1.y());
    xml.properties["x2"] = std::to_string(p2.x());
    xml.properties["y2"] = std::to_string(p2.y());
    this->addStylesToXML(this, xml);
    return xml;
}


void Line::updateShapeOnDraw(QPointF start, QPointF cur)
{
    this->setPolygon(QPolygonF(QList<QPointF> {
        start, cur
    }));
}
