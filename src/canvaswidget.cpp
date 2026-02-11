#include "canvaswidget.h"
#include <QMoveEvent>
#include <QPointF>
#include <QPen>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>

CanvasWidget::CanvasWidget(QWidget* parent)
    : QGraphicsView(parent)
{
    this->setMouseTracking(true);
    this->setBackgroundBrush(QBrush(QColor::fromString("#888888")));
}

void CanvasWidget::CentreOn(float x, float y)
{
    this->centerOn(x, y);
}


void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    QPointF pos = this->mapToScene(event->pos());
    emit MousePosition(pos.x(), pos.y());   // Signal the mouse position has changed on the canvas
}
