#include "graphicscene.h"
#include "propertiesdock.h"
#include "shapes.h"
#include "xmlparser.h"
#include <QGraphicsItem>
#include <QtCore/qpoint.h>
#include <QtWidgets/qwidget.h>
#include <string>



Circle::Circle(float cx, float cy, float rad)
    : QGraphicsEllipseItem(cx-rad, cy-rad, rad*2, rad*2)
{}

Circle::Circle(const XMLTag& xml)
    : QGraphicsEllipseItem()
{
    UpdateFromXML(xml);
}

void Circle::UpdateFromXML(const XMLTag& xml)
{
    float cx = stof(xml.properties.at("cx"));
    float cy = stof(xml.properties.at("cy"));
    float rad = stof(xml.properties.at("r"));
    this->setRect(cx-rad, cy-rad, rad*2, rad*2);
    this->LoadStylesFromXML(this, xml);
}

XMLTag Circle::ToXML() const
{
    float rad = this->rect().height()/2;
    float cx = rect().topLeft().x() + rad;
    float cy = rect().topLeft().y() + rad;
    XMLTag xml(false);
    xml.name = "circle";
    xml.properties["r"] = std::to_string(rad);
    xml.properties["cx"] = std::to_string(cx);
    xml.properties["cy"] = std::to_string(cy);
    this->AddStylesToXML(this, xml);
    return xml;
}

void Circle::UpdateShapeOnDraw(QPointF start, QPointF cur)
{
    float r = QLineF(start, cur).length();
    this->setRect(start.x()-r, start.y()-r, 2*r, 2*r);
}


PropertiesForm* Circle::GetPropertyForm(QWidget* parent)
{
    return new CirclePropForm(parent, static_cast<GraphicScene*>(this->scene()), this);
}
