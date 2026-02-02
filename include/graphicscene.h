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


class GraphicScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicScene(QWidget* parent = nullptr);

signals:
    void centreCanvasOn(float x, float y);

public slots:
    void setViewportRect(float w, float h);
    void addShape(QAbstractGraphicsShapeItem* shape);
    void setToolType(ToolType tool);
    void deleteShape(QAbstractGraphicsShapeItem* shape);
    void addDeletedShape(QAbstractGraphicsShapeItem* shape);
    void deleteSelectedItem();
    void removeAllItems();
    void undo() { undoStack.undo(); }
    void redo() { undoStack.redo(); }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsRectItem *viewportRect;
    QAbstractGraphicsShapeItem *ghostItem;
    std::set<QAbstractGraphicsShapeItem*> shapes;
    std::map<QAbstractGraphicsShapeItem*, std::unique_ptr<QAbstractGraphicsShapeItem> > deletedShapes; 
    ToolType curTool;
    bool drawShapeStarted = false;
    QPointF startDrawPoint;
    UndoStack undoStack;
};


