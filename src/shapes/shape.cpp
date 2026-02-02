#include "shapes.h"
#include "xmlparser.h"
#include <QBrush>
#include <QColor>
#include <QString>
#include <QPen>
#include <QtWidgets/qgraphicsitem.h>
#include <string>

void Shape::addStylesToXML(const QAbstractGraphicsShapeItem* item, XMLTag& xml)
{
    std::string fill_color = item->brush().color().name().toStdString();
    std::string stroke_color = item->pen().brush().color().name().toStdString();
    float stroke_width = item->pen().width();
    xml.properties["fill"] = fill_color;
    xml.properties["stroke"] = stroke_color;
    xml.properties["stroke-width"] = std::to_string(stroke_width);
}


void Shape::loadStylesFromXML(QAbstractGraphicsShapeItem *item, const XMLTag &xml)
{
    if(xml.properties.find("fill") != xml.properties.end())
    {
        Shape::setFillColor(item, xml.properties.at("fill"));
    }
    if(xml.properties.find("stroke") != xml.properties.end())
    {
        Shape::setStrokeColor(item, xml.properties.at("stroke"));
    }
    if(xml.properties.find("stroke-width") != xml.properties.end())
    {
        Shape::setStrokeWidth(item, std::stof(xml.properties.at("stroke-width")));
    }
}


void Shape::setFillColor(QAbstractGraphicsShapeItem *item, const std::string &color)
{
    item->setBrush(QBrush(QColor::fromString(QString::fromStdString(color))));
}

void Shape::setStrokeColor(QAbstractGraphicsShapeItem *item, const std::string &color)
{
    item->setPen(QPen(QBrush(QColor::fromString(QString::fromStdString(color))), item->pen().widthF()));
}

void Shape::setStrokeWidth(QAbstractGraphicsShapeItem *item, float width)
{
    item->setPen(QPen(item->pen().brush(), width));
}
