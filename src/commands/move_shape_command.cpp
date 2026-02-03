#include "commands.h"
#include <QtCore/qpoint.h>

MoveShapeCommand::MoveShapeCommand(QGraphicsItem* item, QPointF startPos, QPointF endPos)
    : item(item), startPos(startPos), endPos(endPos)
{
    item->setPos(endPos);
}

void MoveShapeCommand::undo()
{
    item->setPos(startPos);
}

void MoveShapeCommand::redo()
{
    item->setPos(endPos);
}
