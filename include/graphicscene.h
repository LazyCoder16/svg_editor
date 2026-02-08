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
    void AddShape(std::unique_ptr<QAbstractGraphicsShapeItem> shape);
    // Helpers for undo and redo of delete operation
    void DeleteShape(QAbstractGraphicsShapeItem* shape);
    void AddDeletedShape(QAbstractGraphicsShapeItem* shape);
    // Simple inline getters and setters
    QGraphicsRectItem* GetViewPortRect() { return viewport_rect_; }
    std::set<QAbstractGraphicsShapeItem*> GetDrawnShapes() { return shapes_; }
    void AddAction(std::unique_ptr<Command> command) { undo_stack_.AddAction(std::move(command)); }
    void SetSaved() { undo_stack_.SetClean(); }
    bool IsSaved() { return undo_stack_.IsClean(); }
    UndoStack* GetUndoStack() { return &undo_stack_; }

signals:
    void CentreCanvasOn(float x, float y);
    void SelectedShapesChanged(const std::vector<QAbstractGraphicsShapeItem*>& selectedShapes);

public slots:
    void SetViewportRect(float w, float h);
    void SetToolType(ToolType tool);
    void DeleteSelectedItem();
    void RemoveAllItems();
    // So that other objects can access the undo stack object of the scene
    void Undo() { undo_stack_.Undo(); }
    void Redo() { undo_stack_.Redo(); }
    // Clipboard actions
    void CopyAction();
    void CutAction();
    void PasteAction();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsRectItem *viewport_rect_;
    QAbstractGraphicsShapeItem *ghost_item_;
    std::set<QAbstractGraphicsShapeItem*> shapes_;
    std::map<QAbstractGraphicsShapeItem*, std::unique_ptr<QAbstractGraphicsShapeItem> > deleted_shapes_; 
    ToolType cur_tool_;
    bool draw_shape_started_ = false;
    bool drag_started_ = false;
    QPointF start_draw_point_;
    QPointF start_drag_pos_;
    UndoStack undo_stack_;
};
