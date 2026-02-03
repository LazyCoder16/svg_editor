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
#include <QtGui/qtransform.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QGraphicsSceneDragDropEvent>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <algorithm>
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
    // Let other objects track for a selection change
    connect(this, &GraphicScene::selectionChanged, this, [=]() {
        auto selectedItems = this->selectedItems();
        std::vector<QAbstractGraphicsShapeItem*> selectedShapes;
        for(auto item : selectedItems)
        {
            selectedShapes.push_back(static_cast<QAbstractGraphicsShapeItem*>(item));
        }
        emit selectedShapesChanged(selectedShapes);
    });
}


void GraphicScene::setViewportRect(float w, float h)
{
    this->viewportRect->setRect(0, 0, w, h);
    emit centreCanvasOn(w/2, h/2);
}

void GraphicScene::addShape(std::unique_ptr<QAbstractGraphicsShapeItem> shape)
{
    // Add a brand new shape to our scene
    // Argument is a unique_ptr as we dont want raw point not owned by anyone for memory safety
    // The scene takes ownership of this unique_ptr with the addItem function
    shapes.insert(shape.get());
    this->addItem(shape.release());
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
        shape->setFlag(QGraphicsItem::ItemIsMovable, false);
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
        if(curTool == ToolType::Select)
        {
            shape->setFlag(QGraphicsItem::ItemIsSelectable, true);
            shape->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
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
    // Calls the deleteShape() function in its constructor
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
    // Make a new unique_ptr and give its ownership to the scene after creating it
    // ghostItem pointer will store the object location
    std::unique_ptr<QAbstractGraphicsShapeItem> ghostGuard;
    if(event->button() == Qt::LeftButton && curTool != ToolType::Select) {
        startDrawPoint = event->scenePos();
        for(const auto shape : shapes)
        {
            shape->setFlag(QGraphicsItem::ItemIsSelectable, false);
        }
        this->drawShapeStarted = true;
        if(curTool == ToolType::Rectangle) {
            ghostGuard = std::make_unique<Rectangle>(startDrawPoint.x(), startDrawPoint.y(), 0, 0);
        }
        else if(curTool == ToolType::Circle) {
            ghostGuard = std::make_unique<Circle>(startDrawPoint.x(), startDrawPoint.y(), 0);
        }
        else if(curTool == ToolType::Hexagon) {
            ghostGuard = std::make_unique<Polygon>(std::vector<QPointF> {});
        }
        else if(curTool == ToolType::Line) {
            ghostGuard = std::make_unique<Line>(startDrawPoint.x(), startDrawPoint.y(), startDrawPoint.x(), startDrawPoint.y());
        }
        else if(curTool == ToolType::Freehand) {
            ghostGuard = std::make_unique<FreehandPath>(QPainterPath(startDrawPoint));
        }
        else if(curTool == ToolType::Text) {
            ghostGuard = std::make_unique<Rectangle>(startDrawPoint.x(), startDrawPoint.y(), 0, 0);
            ghostGuard->setPen(QPen(Qt::PenStyle::DotLine));
        }
        ghostItem = ghostGuard.get();
        this->addItem(ghostGuard.release()); // Scene owns the item now
    }
    if(event->button() == Qt::LeftButton && curTool == ToolType::Select)
    {
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if(item) 
        {
            this->startDragPos = item->pos();
            this->dragStarted = true;
        }
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
        if(curTool == ToolType::Text)
        {
            // Edge case of text; our ghostItem is a Rectangle*. We safely remove it from the scene
            // and swap it for a new TextShape* whose ownership is safely transferred to the scene
            auto topLeft = ghostItem->boundingRect().topLeft();
            auto textShape = std::make_unique<TextShape>(topLeft.x(), topLeft.y(), "Edit Text");
            this->removeItem(ghostItem);
            delete ghostItem;
            ghostItem = textShape.get();
            this->addItem(textShape.release());
        }
        this->shapes.insert(ghostItem);
        undoStack.addAction(std::make_unique<AddDeleteShapeCommand>(
            std::vector<QAbstractGraphicsShapeItem*>{ghostItem}, this, true
        ));
        this->drawShapeStarted = false;
        ghostItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        ghostItem->setSelected(true);
        ghostItem = nullptr;
    }
    else if(event->button() == Qt::LeftButton && curTool == ToolType::Select && dragStarted) {
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if(item && item->pos() != startDragPos) {
            undoStack.addAction(std::make_unique<MoveShapeCommand>(
                item, startDragPos, item->pos()
            ));
        }
        dragStarted = false;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}
