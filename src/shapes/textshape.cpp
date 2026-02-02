#include "shapes.h"
#include "xmlparser.h"
#include <QGraphicsItem>
#include <QString>
#include <string>


TextShape::TextShape(float x, float y, const std::string& text)
    : QGraphicsSimpleTextItem()
{
    this->setPos(x, y);
    this->setText(QString::fromStdString(text));
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}


TextShape::TextShape(const XMLTag& xml)
    : QGraphicsSimpleTextItem()
{
    float x = std::stof(xml.properties.at("x"));
    float y = std::stof(xml.properties.at("y"));
    std::string text = xml.children.empty() ? "" : xml.children[0].name;
    this->setPos(x, y);
    this->setText(QString::fromStdString(text));
    this->loadStylesFromXML(this, xml);
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}


XMLTag TextShape::toXML() const
{
    XMLTag xml(false);
    xml.properties["x"] = std::to_string(this->pos().x());
    xml.properties["y"] = std::to_string(this->pos().y());
    this->addStylesToXML(this, xml);
    xml.children.push_back(XMLTag(true));
    xml.children[0].name = this->text().toStdString();
    return xml;
}

