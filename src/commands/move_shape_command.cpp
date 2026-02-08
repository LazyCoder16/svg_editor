#include "commands.h"
#include <QtCore/qpoint.h>

MoveShapeCommand::MoveShapeCommand(QGraphicsItem* item, QPointF startPos, QPointF endPos)
    : item_(item), start_pos_(startPos), end_pos_(endPos)
{
    item->setPos(endPos);
}

void MoveShapeCommand::Undo()
{
    item_->setPos(start_pos_);
}

void MoveShapeCommand::Redo()
{
    item_->setPos(end_pos_);
}
