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
 /*
void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(event->button() == Qt::LeftButton) {
        this->top_left = this->mapToScene(event->pos());
    }
}*/

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    QPointF pos = this->mapToScene(event->pos());
    emit MousePosition(pos.x(), pos.y());
}

/*
void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        QPointF bottom_right = this->mapToScene(event->pos());
        this->scene->addRect(QRectF(top_left, bottom_right), QPen(Qt::black), QBrush(Qt::black));
    }
}*/
