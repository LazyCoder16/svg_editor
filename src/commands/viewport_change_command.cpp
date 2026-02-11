#include "commands.h"
#include <QtWidgets/qgraphicsitem.h>


/*
Change viewport rectangle's height and width
*/
ViewportChangeCommand::ViewportChangeCommand(QGraphicsRectItem* viewport, float ow, float oh, float w, float h)
    : viewport_(viewport), ow_(ow), oh_(oh), w_(w), h_(h)
{
    viewport->setRect(QRectF(0, 0, w, h));
}

void ViewportChangeCommand::Undo()
{
    viewport_->setRect(QRectF(0, 0, ow_, oh_));
}

void ViewportChangeCommand::Redo()
{
    viewport_->setRect(QRectF(0, 0, w_, h_));
}
