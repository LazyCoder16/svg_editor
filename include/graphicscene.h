#pragma once

#include "Enums.h"
#include "undostack.h"
#include <QGraphicsScene>
#include <QWidget>
#include <QtCore/qpoint.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QGraphicsItem>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsRectItem>
#include <set>
#include <vector>


class GraphicScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicScene(QWidget* parent = nullptr);
    void addAction(std::unique_ptr<Command> command) { undoStack.addAction(std::move(command)); }
    void addShape(std::unique_ptr<QAbstractGraphicsShapeItem> shape);
    // Helpers for undo and redo of delete operation
    void deleteShape(QAbstractGraphicsShapeItem* shape);
    void addDeletedShape(QAbstractGraphicsShapeItem* shape);

    std::set<QAbstractGraphicsShapeItem*> shapes;
    QGraphicsRectItem *viewportRect;
    UndoStack undoStack;

signals:
    void centreCanvasOn(float x, float y);
    void selectedShapesChanged(const std::vector<QAbstractGraphicsShapeItem*>& selectedShapes);

public slots:
    void setViewportRect(float w, float h);
    void setToolType(ToolType tool);
    void deleteSelectedItem();
    void removeAllItems();
    // So that other objects can access the undo stack object of the scene
    void undo() { undoStack.undo(); }
    void redo() { undoStack.redo(); }
    // Clipboard actions
    void copyAction();
    void cutAction();
    void pasteAction();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QAbstractGraphicsShapeItem *ghostItem;
    std::map<QAbstractGraphicsShapeItem*, std::unique_ptr<QAbstractGraphicsShapeItem> > deletedShapes; 
    ToolType curTool;
    bool drawShapeStarted = false;
    bool dragStarted = false;
    QPointF startDrawPoint;
    QPointF startDragPos;
};
