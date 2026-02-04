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


/*
Base Shape Class with static helpers and common attributes for styles
*/
class Shape
{
public:
    virtual ~Shape() {}
    virtual XMLTag toXML() const = 0;
    virtual void updateFromXML(const XMLTag& xml) = 0;
    virtual void updateShapeOnDraw(QPointF start, QPointF cur) {}

    static void addStylesToXML(const QAbstractGraphicsShapeItem* item, XMLTag& xml);
    static void setStrokeColor(QAbstractGraphicsShapeItem* item, const std::string& color);
    static void setStrokeWidth(QAbstractGraphicsShapeItem* item, float width);
    static void setFillColor(QAbstractGraphicsShapeItem* item, const std::string& color);
    static void loadStylesFromXML(QAbstractGraphicsShapeItem* item, const XMLTag& xml);
};


/*
Children classes inheriting from Shape and QAbstractGraphicsShapeItem class for rendering
*/
class Rectangle : public Shape, public QGraphicsRectItem
{
public:
    Rectangle(float x, float y, float w, float h);
    Rectangle(const XMLTag& xml);
    XMLTag toXML() const override;
    void updateFromXML(const XMLTag& xml) override;
    void updateShapeOnDraw(QPointF start, QPointF cur) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    float rx=0, ry=0; // For rounded rectangle functionality
};


class Circle : public Shape, public QGraphicsEllipseItem
{
public:
    Circle(float cx, float cy, float rad);
    Circle(const XMLTag& xml);
    void updateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag toXML() const override;
    void updateFromXML(const XMLTag& xml) override;
};


class TextShape : public Shape, public QGraphicsSimpleTextItem
{
public:
    TextShape(float x, float y, const std::string& text);
    TextShape(const XMLTag& xml);
    XMLTag toXML() const override;
    void updateFromXML(const XMLTag& xml) override;
};


class Polygon : public Shape, public QGraphicsPolygonItem
{
public:
    Polygon(const std::vector<QPointF>& points);
    Polygon(const XMLTag& xml);
    void updateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag toXML() const override;
    void updateFromXML(const XMLTag& xml) override;
};

class Line : public Shape, public QGraphicsPolygonItem
{
public:
    Line(float x1, float y1, float x2, float y2);
    Line(const XMLTag& xml);
    void updateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag toXML() const override;
    void updateFromXML(const XMLTag& xml) override;
};


class FreehandPath : public Shape, public QGraphicsPathItem
{
public:
    FreehandPath(const QPainterPath& path);
    FreehandPath(const XMLTag& xml);
    void updateShapeOnDraw(QPointF start, QPointF cur) override;
    XMLTag toXML() const override;
    void updateFromXML(const XMLTag& xml) override;
};

#endif
