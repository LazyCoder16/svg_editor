#include "xmlparser.h"
#include "shapes.h"
#include <QGraphicsItem>
#include <QList>
#include <QPointF>
#include <QtCore/qpoint.h>
#include <QtGui/qpainterpath.h>
#include <QtWidgets/qgraphicsitem.h>
#include <cctype>
#include <string>
#include <sstream>


QPainterPath parseSVGPath(const std::string& d) {
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
{
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

FreehandPath::FreehandPath(const XMLTag& xml)
    : QGraphicsPathItem(parseSVGPath(xml.properties.at("d")))
{
    this->loadStylesFromXML(this, xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

XMLTag FreehandPath::toXML() const
{
    XMLTag xml(false);
    xml.name = "path";
    this->addStylesToXML(this, xml);
    return xml;
}


void FreehandPath::updateShapeOnDraw(QPointF start, QPointF cur)
{
    auto path = this->path();
    if(path.elementCount() == 0) {
        path.moveTo(start);
    }
    path.lineTo(cur);
    this->setPath(path);
}
