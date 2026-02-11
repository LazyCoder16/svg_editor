#include "propertiesdock.h"
#include "xmlparser.h"
#include "shapes.h"
#include <QGraphicsItem>
#include <QList>
#include <QPointF>
#include <QtCore/qpoint.h>
#include <QtGui/qpainterpath.h>
#include <QtGui/qpolygon.h>
#include <QtWidgets/qgraphicsitem.h>
#include <cctype>
#include <string>
#include <sstream>


QPainterPath ParseSVGPath(const std::string& d) {
    // Parses the svg attribute "d" supports only M and L instructions
    // Also supports the short notation M x1 y1 x2 y2 x3 y3... common in freehand tools
    QPainterPath path;
    std::stringstream ss(d);
    char command = 'M';
    float x, y;
    while(ss >> std::ws && !ss.eof())
    {
        if(isalpha(ss.peek())) 
        {
            ss >> command;
            command = std::toupper(command);
        }
        if(ss >> x >> y) 
        {
            if(command == 'M') 
            {
                path.moveTo(x, y);
                command = 'L'; 
            } 
            else if(command == 'L') 
            {
                path.lineTo(x, y);
            }
        }
    }
    return path;
}

FreehandPath::FreehandPath(const QPainterPath& path)
    : QGraphicsPathItem(path)
{}

FreehandPath::FreehandPath(const XMLTag& xml)
    : QGraphicsPathItem()
{
    this->UpdateFromXML(xml);
}

void FreehandPath::UpdateFromXML(const XMLTag& xml)
{
    this->setPath(ParseSVGPath(xml.properties.at("d")));
    this->LoadStylesFromXML(this, xml);
}

XMLTag FreehandPath::ToXML() const
{
    XMLTag xml(false);
    xml.name = "path";
    QPolygonF polygon = this->path().toFillPolygon();
    std::string xml_path;
    // Convert path back to SVG format
    for(QPointF point : polygon)
    {
        if(xml_path.empty())
        {
            xml_path += "M ";
        }
        xml_path += std::to_string(point.x()) + " " + std::to_string(point.y()) + " ";
    }
    xml.properties["d"] = xml_path;
    this->AddStylesToXML(this, xml);
    return xml;
}


void FreehandPath::UpdateShapeOnDraw(QPointF start, QPointF cur)
{
    auto path = this->path();
    if(path.elementCount() == 0) {
        path.moveTo(start);
    }
    path.lineTo(cur);
    this->setPath(path);
}

PropertiesForm* FreehandPath::GetPropertyForm(QWidget* parent)
{
    return new FreehandPropForm(parent, static_cast<GraphicScene*>(this->scene()), this);
}

