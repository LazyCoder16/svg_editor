#include "shapes.h"
#include "xmlparser.h"
#include <QBrush>
#include <QColor>
#include <QString>
#include <QPen>
#include <QtCore/qnamespace.h>
#include <QtGui/qcolor.h>
#include <QtGui/qpen.h>
#include <QtWidgets/qgraphicsitem.h>
#include <string>

void Shape::addStylesToXML(const QAbstractGraphicsShapeItem* item, XMLTag& xml)
{
    std::string fill_color = item->brush().color().name(QColor::HexArgb).toStdString();
    if(item->brush() == Qt::NoBrush) {
        fill_color = "#00000000";
    }
    float opacity = std::stoi(fill_color.substr(1, 2), nullptr, 16) / 255.0;
    std::string stroke_color = item->pen().brush().color().name().toStdString();
    float stroke_width = item->pen().width();
    if(item->pen().style() == Qt::NoPen) {
        stroke_width = 0;
    }
    xml.properties["fill"] = "#" + fill_color.substr(3);
    xml.properties["stroke"] = stroke_color;
    xml.properties["stroke-width"] = std::to_string(stroke_width);
    xml.properties["fill-opacity"] = std::to_string(opacity);
}


void Shape::loadStylesFromXML(QAbstractGraphicsShapeItem *item, const XMLTag &xml)
{
    int opacity = 255;
    if(xml.properties.find("fill-opacity") != xml.properties.end())
    {
        opacity = 255 * stof(xml.properties.at("fill-opacity"));
    }
    if(xml.properties.find("fill") != xml.properties.end())
    {
        std::string ophex = QString("%1").arg(opacity, 2, 16, QChar('0')).toStdString();
        Shape::setFillColor(item, "#" + ophex + xml.properties.at("fill").substr(1));
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
