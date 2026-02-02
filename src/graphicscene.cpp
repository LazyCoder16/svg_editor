#include "graphicscene.h"
#include "Enums.h"
#include "commands.h"
#include "shapes.h"
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QtCore/qnamespace.h>
#include <QtCore/qpoint.h>
#include <QtGui/qbrush.h>
#include <QtGui/qpainterpath.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QGraphicsSceneDragDropEvent>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <memory>
#include <vector>


GraphicScene::GraphicScene(QWidget* parent)
    : QGraphicsScene(parent)
{
    this->setSceneRect(-2500, -1000, 5000, 2000);
    viewportRect = this->addRect(0, 0, 128, 128, QPen(), QBrush(Qt::white));
    emit centreCanvasOn(viewportRect->rect().width()/2, viewportRect->rect().height()/2);
}


/*
Implementation of slots
*/
void GraphicScene::setViewportRect(float w, float h)
{
    this->viewportRect->setRect(0, 0, w, h);
    emit centreCanvasOn(w/2, h/2);
}

void GraphicScene::addShape(QAbstractGraphicsShapeItem* shape)
{
    // Add a brand new shape to our scene
    this->addItem(shape);
    shapes.insert(shape);
}

void GraphicScene::setToolType(ToolType tool)
{
    this->curTool = tool;
    bool flag = (curTool == ToolType::Select);
    for(auto shape : this->shapes)
    {
        shape->setFlag(QGraphicsItem::ItemIsSelectable, flag);
        shape->setFlag(QGraphicsItem::ItemIsMovable, flag);
    }
}

void GraphicScene::deleteShape(QAbstractGraphicsShapeItem* shape)
{
    // Remove from pool of currently active shapes and add to deletedShapes for memory
    if(shapes.find(shape) != shapes.end())
    {
        shape->setFlag(QGraphicsItem::ItemIsSelectable, false);
        auto ushape = std::unique_ptr<QAbstractGraphicsShapeItem>(shape); // Scene is leaving the pointer's ownership
        this->removeItem(shape);
        shapes.erase(shape);
        deletedShapes[shape] = std::move(ushape); // deletedShapes owns the pointer
    }
}

void GraphicScene::addDeletedShape(QAbstractGraphicsShapeItem* shape)
{
    // Add a removed shape back into the canvas
    if(deletedShapes.find(shape) != deletedShapes.end())
    {
        this->addItem(deletedShapes[shape].release()); // We can safely pass it back to the scene
        deletedShapes.erase(shape); // Delete on null unique_ptr is safe
        shapes.insert(shape);
    }
}

void GraphicScene::deleteSelectedItem()
{
    std::vector<QAbstractGraphicsShapeItem*> shapes;
    for(auto item : this->selectedItems())
    {
        shapes.push_back(static_cast<QAbstractGraphicsShapeItem*>(item));
    }
    undoStack.addAction(std::make_unique<AddDeleteShapeCommand>(
        shapes, this, false
    ));
}

void GraphicScene::removeAllItems()
{
    undoStack.reset();
    for(auto shape : shapes) 
    {
        this->removeItem(shape);
        delete shape; // Necessary
    }
    deletedShapes.clear(); // Deletes all the pointers it owns
    shapes.clear();
}

/*
Mouse events from drawing new shape
*/
void GraphicScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && curTool != ToolType::Select && curTool != ToolType::Text) {
        for(const auto shape : shapes)
        {
            shape->setFlag(QGraphicsItem::ItemIsSelectable, false);
        }
        startDrawPoint = event->scenePos();
        this->drawShapeStarted = true;
        if(curTool == ToolType::Rectangle) {
            ghostItem = new Rectangle(startDrawPoint.x(), startDrawPoint.y(), 0, 0);
        }
        else if(curTool == ToolType::Circle) {
            ghostItem = new Circle(startDrawPoint.x(), startDrawPoint.y(), 0);
        }
        else if(curTool == ToolType::Hexagon) {
            ghostItem = new Polygon(std::vector<QPointF> {});
        }
        else if(curTool == ToolType::Line) {
            ghostItem = new Line(startDrawPoint.x(), startDrawPoint.y(), startDrawPoint.x(), startDrawPoint.y());
        }
        else if(curTool == ToolType::Freehand) {
            ghostItem = new FreehandPath(QPainterPath(startDrawPoint));
        }
        this->addItem(ghostItem); // Scene owns the item now
    }
    QGraphicsScene::mousePressEvent(event);
}

void GraphicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->drawShapeStarted) {
        QPointF curPoint = event->scenePos();
        dynamic_cast<Shape*>(ghostItem)->updateShapeOnDraw(startDrawPoint, curPoint);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && curTool != ToolType::Select && drawShapeStarted) {
        undoStack.addAction(std::make_unique<AddDeleteShapeCommand>(
            std::vector<QAbstractGraphicsShapeItem*>{ghostItem}, this, true
        ));
        this->drawShapeStarted = false;
        ghostItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        ghostItem->setSelected(true);
        ghostItem = nullptr;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}
