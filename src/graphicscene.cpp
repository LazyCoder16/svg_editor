#include "graphicscene.h"
#include "Enums.h"
#include "commands.h"
#include "doccontroller.h"
#include "shapes.h"
#include "xmlparser.h"
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QtCore/qnamespace.h>
#include <QtCore/qpoint.h>
#include <QtGui/qbrush.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpainterpath.h>
#include <QtGui/qtransform.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QGraphicsSceneDragDropEvent>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QGraphicsSceneMouseEvent>
#include <memory>
#include <sstream>
#include <vector>
#include <QClipboard>


GraphicScene::GraphicScene(QWidget* parent)
    : QGraphicsScene(parent)
{
    this->setSceneRect(-2500, -1000, 5000, 2000);
    viewport_rect_ = this->addRect(0, 0, 128, 128, QPen(), QBrush(Qt::white));
    emit CentreCanvasOn(viewport_rect_->rect().width()/2, viewport_rect_->rect().height()/2);
    // Let other objects track for a selection change
    connect(this, &GraphicScene::selectionChanged, this, [=]() {
        auto selected_items = this->selectedItems();
        std::vector<QAbstractGraphicsShapeItem*> selected_shapes;
        for(auto item : selected_items)
        {
            selected_shapes.push_back(static_cast<QAbstractGraphicsShapeItem*>(item));
        }
        emit SelectedShapesChanged(selected_shapes);  // emit the signal
    });
}


void GraphicScene::SetViewportRect(float w, float h)
{
    this->viewport_rect_->setRect(0, 0, w, h);
    emit CentreCanvasOn(w/2, h/2);
}

void GraphicScene::AddShape(std::unique_ptr<QAbstractGraphicsShapeItem> shape)
{
    // Add a brand new shape to our scene
    // Argument is a unique_ptr as we dont want raw point not owned by anyone for memory safety
    // The scene takes ownership of this unique_ptr with the addItem function
    shapes_.insert(shape.get());
    this->addItem(shape.release());
}

void GraphicScene::SetToolType(ToolType tool)
{
    this->cur_tool_ = tool;
    // Item is selectable and movable if the current tool is Select
    bool flag = (cur_tool_ == ToolType::Select);
    for(auto shape : this->shapes_)
    {
        shape->setFlag(QGraphicsItem::ItemIsSelectable, flag);
        shape->setFlag(QGraphicsItem::ItemIsMovable, flag);
    }
}

void GraphicScene::DeleteShape(QAbstractGraphicsShapeItem* shape)
{
    // Remove from pool of currently active shapes and add to deletedShapes for memory
    if(shapes_.find(shape) != shapes_.end())
    {
        shape->setFlag(QGraphicsItem::ItemIsSelectable, false);
        shape->setFlag(QGraphicsItem::ItemIsMovable, false);
        auto ushape = std::unique_ptr<QAbstractGraphicsShapeItem>(shape); // Scene is leaving the pointer's ownership
        this->removeItem(shape);
        shapes_.erase(shape);
        deleted_shapes_[shape] = std::move(ushape); // deletedShapes owns the pointer
    }
}

void GraphicScene::AddDeletedShape(QAbstractGraphicsShapeItem* shape)
{
    // Add a removed shape back into the canvas
    if(deleted_shapes_.find(shape) != deleted_shapes_.end())
    {
        if(cur_tool_ == ToolType::Select)
        {
            shape->setFlag(QGraphicsItem::ItemIsSelectable, true);
            shape->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
        this->addItem(deleted_shapes_[shape].release()); // We can safely pass it back to the scene
        deleted_shapes_.erase(shape); // Delete on null unique_ptr is safe
        shapes_.insert(shape);
    }
}

void GraphicScene::DeleteSelectedItem()
{
    std::vector<QAbstractGraphicsShapeItem*> shapes;
    for(auto item : this->selectedItems())
    {
        shapes.push_back(static_cast<QAbstractGraphicsShapeItem*>(item));
    }
    // Calls the deleteShape() function in its constructor
    undo_stack_.AddAction(std::make_unique<AddDeleteShapeCommand>(
        shapes, this, false
    ));
}

void GraphicScene::RemoveAllItems()
{
    // Delete all items
    undo_stack_.Reset();  // Reset the stack
    for(auto shape : shapes_) 
    {
        this->removeItem(shape);
        delete shape; // Necessary
    }
    deleted_shapes_.clear(); // Deletes all the pointers it owns
    shapes_.clear();
}

/*
Mouse events from drawing new shape
*/
void GraphicScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Make a new unique_ptr and give its ownership to the scene after creating it
    // ghost_item pointer will store the object location
    std::unique_ptr<QAbstractGraphicsShapeItem> ghost_guard;
    if(event->button() == Qt::LeftButton && cur_tool_ != ToolType::Select) {
        // Drawing a new shape
        start_draw_point_ = event->scenePos();
        for(const auto shape : shapes_)
        {
            shape->setFlag(QGraphicsItem::ItemIsSelectable, false);
        }
        this->draw_shape_started_ = true;
        if(cur_tool_ == ToolType::Rectangle) {
            ghost_guard = std::make_unique<Rectangle>(start_draw_point_.x(), start_draw_point_.y(), 0, 0);
        }
        else if(cur_tool_ == ToolType::Circle) {
            ghost_guard = std::make_unique<Circle>(start_draw_point_.x(), start_draw_point_.y(), 0);
        }
        else if(cur_tool_ == ToolType::Hexagon) {
            ghost_guard = std::make_unique<Polygon>(std::vector<QPointF> {});
        }
        else if(cur_tool_ == ToolType::Line) {
            ghost_guard = std::make_unique<Line>(start_draw_point_.x(), start_draw_point_.y(), start_draw_point_.x(), start_draw_point_.y());
        }
        else if(cur_tool_ == ToolType::Freehand) {
            ghost_guard = std::make_unique<FreehandPath>(QPainterPath(start_draw_point_));
        }
        else if(cur_tool_ == ToolType::Text) {
            // Draw a dummy rectangle if the tool is text
            ghost_guard = std::make_unique<Rectangle>(start_draw_point_.x(), start_draw_point_.y(), 0, 0);
            ghost_guard->setPen(QPen(Qt::PenStyle::DotLine));
        }
        ghost_item_ = ghost_guard.get();
        this->addItem(ghost_guard.release()); // Scene owns the item now
    }
    if(event->button() == Qt::LeftButton && cur_tool_ == ToolType::Select)
    {
        // Moving shape
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if(item) 
        {
            this->start_drag_pos_ = item->pos();
            this->drag_started_ = true;
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void GraphicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->draw_shape_started_) {
        QPointF cur_point = event->scenePos();
        dynamic_cast<Shape*>(ghost_item_)->UpdateShapeOnDraw(start_draw_point_, cur_point);
    }
    QGraphicsScene::mouseMoveEvent(event);  // Handles the shape moving
}

void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && cur_tool_ != ToolType::Select && draw_shape_started_) {
        if(cur_tool_ == ToolType::Text)
        {
            // Edge case of text; our ghostItem is a Rectangle*. We safely remove it from the scene
            // and swap it for a new TextShape* whose ownership is safely transferred to the scene
            auto top_left = ghost_item_->boundingRect().topLeft();
            auto text_shape = std::make_unique<TextShape>(top_left.x(), top_left.y(), "Edit Text");
            this->removeItem(ghost_item_);
            delete ghost_item_;
            ghost_item_ = text_shape.get();
            this->addItem(text_shape.release());
        }
        this->shapes_.insert(ghost_item_);
        // Add this action onto the undo stack
        undo_stack_.AddAction(std::make_unique<AddDeleteShapeCommand>(
            std::vector<QAbstractGraphicsShapeItem*>{ghost_item_}, this, true
        ));
        this->draw_shape_started_ = false;
        ghost_item_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        ghost_item_->setSelected(true);
        ghost_item_ = nullptr;
    }
    else if(event->button() == Qt::LeftButton && cur_tool_ == ToolType::Select && drag_started_) {
        // Item has finished moving from its original position
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if(item && item->pos() != start_drag_pos_) {
            undo_stack_.AddAction(std::make_unique<MoveShapeCommand>(
                item, start_drag_pos_, item->pos()
            ));
        }
        drag_started_ = false;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

/*
Clipboard Actions
*/
void GraphicScene::CopyAction()
{
    // Convert selected items to xml format and store the text in qclipboard
    auto selected_items = this->selectedItems();
    if(!selected_items.empty())
    {
        auto clipboard = QGuiApplication::clipboard();
        XMLTag xml;
        xml.name = "svg";
        for(auto item : selected_items) {
            auto shape = dynamic_cast<Shape*>(item);
            xml.children.push_back(shape->ToXML());
        }
        std::stringstream ss;
        ss << xml;
        clipboard->setText(QString::fromStdString(ss.str()));
    }
}

void GraphicScene::CutAction()
{
    // First copy the selected shapes then delete them from the scene
    // DeleteSelectedItems adds its action to the undo stack by default
    this->CopyAction();
    this->DeleteSelectedItem();
}

void GraphicScene::PasteAction()
{
    // Add the shapes in the clipboard to the scene after converting from xml format
    std::string clipboard_text = QGuiApplication::clipboard()->text().toStdString();
    if(!clipboard_text.empty())
    {
        for(auto item : this->selectedItems())
        {
            item->setSelected(false);
        }
        XMLTag xml;
        XMLParser::ParseFile(clipboard_text, xml);
        std::vector<std::unique_ptr<QAbstractGraphicsShapeItem>> shapes;
        std::vector<QAbstractGraphicsShapeItem*> raw_shapes;
        DocController::ExtractShapes(xml, shapes);
        for(int i=0; i<shapes.size(); ++i)
        {
            shapes[i]->setPos(20, 20);
            // Pasted items are selected by default
            shapes[i]->setFlag(QGraphicsItem::ItemIsSelectable, true);
            shapes[i]->setFlag(QGraphicsItem::ItemIsMovable, true);
            shapes[i]->setSelected(true);
            raw_shapes.push_back(shapes[i].get());
            this->AddShape(std::move(shapes[i]));
        }
        this->undo_stack_.AddAction(std::make_unique<AddDeleteShapeCommand>(
            raw_shapes, this, true
        ));
    }
}
