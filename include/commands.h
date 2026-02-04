#ifndef COMMANDS_H
#define COMMANDS_H


#include "shapes.h"
#include "xmlparser.h"
#include <QAbstractGraphicsShapeItem>
#include <QtWidgets/qgraphicsitem.h>
#include <vector>
#include <QGraphicsRectItem>
class GraphicScene; // Forward declaration to solve cyclic includes

/*
Defines class interface for each command supported by our interface.
Will be used as the data-type for our undo-redo stacks
implementation is in src/commands folder
*/

// Base Class (interface)
class Command
{
public:
    virtual ~Command() {} // Inheritence safety
    virtual void undo() = 0;
    virtual void redo() = 0;
};

/*
Command for drawing/deleting a shape
*/
class AddDeleteShapeCommand : public Command
{  
public:
    AddDeleteShapeCommand(std::vector<QAbstractGraphicsShapeItem*> items, GraphicScene *scene, bool added);
    void undo() override;
    void redo() override;

private:
    std::vector<QAbstractGraphicsShapeItem*> items;
    GraphicScene *scene;
    bool added;
};

/*
Command for moving a shape
*/
class MoveShapeCommand : public Command
{
public:
    MoveShapeCommand(QGraphicsItem* item, QPointF startPos, QPointF endPos);
    void undo() override;
    void redo() override;

private:
    QGraphicsItem* item;
    QPointF startPos;
    QPointF endPos;
};


class ViewportChangeCommand : public Command
{
public:
    ViewportChangeCommand(QGraphicsRectItem* viewport, float ow, float oh, float w, float h);
    void undo() override;
    void redo() override;

private:
    QGraphicsRectItem *viewport;
    float ow, oh, w, h;
};


class ShapeXMLCommand : public Command
{
public:
    ShapeXMLCommand(Shape* shape, const XMLTag& old, const XMLTag& cur);
    void undo() override;
    void redo() override;

private:
    Shape* shape;
    XMLTag old, cur;
};

#endif // COMMAND_H
