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
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

/*
Command for drawing/deleting a shape
*/
class AddDeleteShapeCommand : public Command
{  
public:
    AddDeleteShapeCommand(std::vector<QAbstractGraphicsShapeItem*> items, GraphicScene *scene, bool added);
    void Undo() override;
    void Redo() override;

private:
    std::vector<QAbstractGraphicsShapeItem*> items_;
    GraphicScene *scene_;
    bool added_;
};

/*
Command for moving a shape
*/
class MoveShapeCommand : public Command
{
public:
    MoveShapeCommand(QGraphicsItem* item, QPointF startPos, QPointF endPos);
    void Undo() override;
    void Redo() override;

private:
    QGraphicsItem* item_;
    QPointF start_pos_;
    QPointF end_pos_;
};


class ViewportChangeCommand : public Command
{
public:
    ViewportChangeCommand(QGraphicsRectItem* viewport, float ow, float oh, float w, float h);
    void Undo() override;
    void Redo() override;

private:
    QGraphicsRectItem *viewport_;
    float ow_, oh_, w_, h_;
};


class ShapeXMLCommand : public Command
{
public:
    ShapeXMLCommand(Shape* shape, const XMLTag& old, const XMLTag& cur);
    void Undo() override;
    void Redo() override;

private:
    Shape* shape_;
    XMLTag old_, cur_;
};

#endif // COMMAND_H
