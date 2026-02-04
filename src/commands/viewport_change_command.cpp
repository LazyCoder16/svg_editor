#include "commands.h"
#include <QtWidgets/qgraphicsitem.h>


ViewportChangeCommand::ViewportChangeCommand(QGraphicsRectItem* viewport, float ow, float oh, float w, float h)
    : viewport(viewport), ow(ow), oh(oh), w(w), h(h)
{
    viewport->setRect(QRectF(0, 0, w, h));
}

void ViewportChangeCommand::undo()
{
    viewport->setRect(QRectF(0, 0, ow, oh));
}

void ViewportChangeCommand::redo()
{
    viewport->setRect(QRectF(0, 0, w, h));
}
