#include "propertiesdock.h"
#include "shapes.h"
#include "xmlparser.h"
#include <QGraphicsItem>
#include <QString>
#include <QtCore/qnamespace.h>
#include <QtGui/qpainterpath.h>
#include <string>
#include <QFont>
#include <QPen>


TextShape::TextShape(float x, float y, const std::string& text)
    : QGraphicsSimpleTextItem()
{
    this->setPos(x, y);
    this->setText(QString::fromStdString(text));
}


TextShape::TextShape(const XMLTag& xml)
    : QGraphicsSimpleTextItem()
{
    this->UpdateFromXML(xml);
}

void TextShape::UpdateFromXML(const XMLTag& xml)
{
    float x = std::stof(xml.properties.at("x"));
    float y = std::stof(xml.properties.at("y"));
    if(xml.properties.find("font-size") != xml.properties.end())
    {
        float font_size = std::stof(xml.properties.at("font-size"));
        QFont font;
        font.setPointSizeF(font_size);
        this->setFont(font);
    }
    std::string text = xml.children.empty() ? "" : xml.children[0].name;
    this->setPos(x, y);
    this->setText(QString::fromStdString(text));
    this->LoadStylesFromXML(this, xml);
}


XMLTag TextShape::ToXML() const
{
    XMLTag xml(false);
    xml.name = "text";
    xml.properties["x"] = std::to_string(this->pos().x());
    xml.properties["y"] = std::to_string(this->pos().y());
    xml.properties["font-size"] = std::to_string(this->font().pointSizeF());
    this->AddStylesToXML(this, xml);
    if(this->pen().style() == Qt::NoPen)
    {
        xml.properties["stroke-width"] = std::to_string(0);
    }
    // Add the text as a child of the main xml tag representing the text shape
    xml.children.push_back(XMLTag(true));
    xml.children[0].name = this->text().toStdString();
    return xml;
}

PropertiesForm* TextShape::GetPropertyForm(QWidget* parent)
{
    return new TextPropForm(parent, static_cast<GraphicScene*>(this->scene()), this);
}


