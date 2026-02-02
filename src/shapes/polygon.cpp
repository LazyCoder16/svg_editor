#include "shapes.h"
#include "xmlparser.h"
#include <QGraphicsItem>
#include <QPolygonF>
#include <QList>
#include <QtCore/qline.h>
#include <QtCore/qpoint.h>
#include <QtGui/qpolygon.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>


std::vector<std::string> split(const std::string& s, char del)
{
    std::string buffer;
    std::vector<std::string> v;
    for(char ch : s)
    {
        if(ch == del)
        {
            v.push_back(buffer);
            buffer.clear();
        }
        else 
        {
            buffer.push_back(ch);
        }
    }
    v.push_back(buffer);
    return v;
}

std::string points_to_string(const QList<QPointF>& points)
{
    std::string s;
    for(QPointF point : points)
    {
        s = s + std::to_string(point.x()) + "," + std::to_string(point.y()) + " ";
    }
    s.pop_back();
    return s;
}

Polygon::Polygon(const std::vector<QPointF>& points)
    : QGraphicsPolygonItem(QList<QPointF>(points.begin(), points.end()))
{
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

Polygon::Polygon(const XMLTag& xml)
    : QGraphicsPolygonItem()
{
    std::vector<QPointF> points;
    const auto& v = split(xml.properties.at("points"), ' ');
    for(const std::string& s : v)
    {
        const auto& v1 = split(s, ',');
        if(v1.size() >= 2)
            points.push_back(QPointF(stof(v1[0]), stof(v1[1])));
    }
    std::cout << "Number of points: " << points.size() << "\n";
    this->setPolygon(QPolygonF(QList<QPointF>(points.begin(), points.end())));
    this->loadStylesFromXML(this, xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

XMLTag Polygon::toXML() const
{
    XMLTag xml(false);
    xml.properties["points"] = points_to_string(this->polygon().toList());
    this->addStylesToXML(this, xml);
    return xml;
}

void Polygon::updateShapeOnDraw(QPointF start, QPointF cur)
{
    float r = QLineF(start, cur).length();
    float phi = std::atan2(cur.y()-start.y(), cur.x()-start.x());
    float theta = M_PI / 3;
    std::vector<QPointF> points;
    for(int _=0; _<6; ++_)
    {
        float x = start.x() + (r * std::cos(phi));
        float y = start.y() + (r * std::sin(phi));
        points.push_back(QPointF(x, y));
        phi += theta;
    }
    this->setPolygon(QPolygonF(QList<QPointF>(points.begin(), points.end())));
}
