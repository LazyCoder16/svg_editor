#include "commands.h"


ShapeXMLCommand::ShapeXMLCommand(Shape* shape, const XMLTag& old, const XMLTag& cur)
    : shape_(shape), old_(old), cur_(cur)
{
    shape->UpdateFromXML(cur);
}

void ShapeXMLCommand::Undo()
{
    shape_->UpdateFromXML(old_);
}

void ShapeXMLCommand::Redo()
{
    shape_->UpdateFromXML(cur_);
}
