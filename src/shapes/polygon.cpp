#include "propertiesdock.h"
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
#include <sstream>


std::vector<std::string> SplitString(const std::string& s, char del)
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

std::string PointToStr(const QList<QPointF>& points)
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
    this->UpdateFromXML(xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

void Polygon::UpdateFromXML(const XMLTag& xml)
{
    std::vector<QPointF> points;
    std::stringstream ss(xml.properties.at("points"));
    std::string s;
    while(ss >> s)
    {
        const auto& v1 = SplitString(s, ',');
        if(v1.size() >= 2)
            points.push_back(QPointF(stof(v1[0]), stof(v1[1])));
    }
    this->setPolygon(QPolygonF(QList<QPointF>(points.begin(), points.end())));
    this->LoadStylesFromXML(this, xml);
}

XMLTag Polygon::ToXML() const
{
    XMLTag xml(false);
    xml.name = "polygon";
    xml.properties["points"] = PointToStr(this->polygon().toList());
    this->AddStylesToXML(this, xml);
    return xml;
}

void Polygon::UpdateShapeOnDraw(QPointF start, QPointF cur)
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

QPointF Polygon::GetCentroid()
{
    QPointF centroid(0, 0);
    int n = this->polygon().size();
    for(const auto &point : this->polygon())
    {
        centroid += point / n;
    }
    return centroid;
}

float Polygon::GetRadius()
{
    QPointF centroid = this->GetCentroid();
    int n = this->polygon().size();
    float radius = 0;
    for(const auto &point : this->polygon())
    {
        radius += QLineF(centroid, point).length() / n;
    }
    return radius;
}

void Polygon::SetRadius(float newRadius)
{
    float cur_radius = this->GetRadius();
    float scale = newRadius / cur_radius;
    auto centroid = this->GetCentroid();
    QList<QPointF> new_polygon;
    for(const auto &point : this->polygon())
    {
        auto new_point = centroid + ((point - centroid) * scale);
        new_polygon.push_back(new_point);
    }
    this->setPolygon(QPolygonF(new_polygon));
}

PropertiesForm* Polygon::GetPropertyForm(QWidget* parent)
{
    return new PolygonPropForm(parent, static_cast<GraphicScene*>(this->scene()), this);
}

