#ifndef SHAPES_H
#define SHAPES_H
#include "xmlparser.h"
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QtCore/qpoint.h>
#include <string>
#include <vector>
class PropertiesForm;

/*
Base Shape Class with static helpers and common attributes for styles
*/
class Shape
{
public:
    virtual ~Shape() {}
    virtual XMLTag ToXML() const = 0;
    virtual void UpdateFromXML(const XMLTag& xml) = 0;
    virtual void UpdateShapeOnDraw(QPointF start, QPointF cur) {} // Update the live shape given the initial and final position of the user mouse when dragging
    virtual PropertiesForm* GetPropertyForm(QWidget* parent) = 0;

    static void AddStylesToXML(const QAbstractGraphicsShapeItem* item, XMLTag& xml);
    static void SetStrokeColor(QAbstractGraphicsShapeItem* item, const std::string& color);
    static void SetStrokeWidth(QAbstractGraphicsShapeItem* item, float width);
    static void SetFillColor(QAbstractGraphicsShapeItem* item, const std::string& color);
    static void LoadStylesFromXML(QAbstractGraphicsShapeItem* item, const XMLTag& xml);
};


/*
Children classes inheriting from Shape and QAbstractGraphicsShapeItem class for rendering
Implements the virtual functions of Shape class
*/
class Rectangle : public Shape, public QGraphicsRectItem
{
public:
    Rectangle(float x, float y, float w, float h);
    Rectangle(const XMLTag& xml);
    XMLTag ToXML() const override;
    void UpdateFromXML(const XMLTag& xml) override;
    void UpdateShapeOnDraw(QPointF start, QPointF cur) override;
    PropertiesForm* GetPropertyForm(QWidget* parent) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    float rx=0, ry=0; // For rounded rectangle functionality
};


class Circle : public Shape, public QGraphicsEllipseItem
{
public:
    Circle(float cx, float cy, float rad);
    Circle(const XMLTag& xml);
    void UpdateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag ToXML() const override;
    void UpdateFromXML(const XMLTag& xml) override;
    PropertiesForm* GetPropertyForm(QWidget* parent) override;
};


class TextShape : public Shape, public QGraphicsSimpleTextItem
{
public:
    TextShape(float x, float y, const std::string& text);
    TextShape(const XMLTag& xml);
    XMLTag ToXML() const override;
    void UpdateFromXML(const XMLTag& xml) override;
    PropertiesForm* GetPropertyForm(QWidget* parent) override;
};


class Polygon : public Shape, public QGraphicsPolygonItem
{
public:
    Polygon(const std::vector<QPointF>& points);
    Polygon(const XMLTag& xml);
    void UpdateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag ToXML() const override;
    void UpdateFromXML(const XMLTag& xml) override;
    float GetRadius();
    void SetRadius(float rad);
    PropertiesForm* GetPropertyForm(QWidget* parent) override;
private:
    QPointF GetCentroid();
};

class Line : public Shape, public QGraphicsPolygonItem
{
public:
    Line(float x1, float y1, float x2, float y2);
    Line(const XMLTag& xml);
    void UpdateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag ToXML() const override;
    void UpdateFromXML(const XMLTag& xml) override;
    PropertiesForm* GetPropertyForm(QWidget* parent) override;
};


class FreehandPath : public Shape, public QGraphicsPathItem
{
public:
    FreehandPath(const QPainterPath& path);
    FreehandPath(const XMLTag& xml);
    void UpdateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag ToXML() const override;
    void UpdateFromXML(const XMLTag& xml) override;
    PropertiesForm* GetPropertyForm(QWidget* parent) override;
};

#endif
