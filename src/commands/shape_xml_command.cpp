#include "commands.h"


/*
Call the Shape class virtual functions to serialize into XML and reverse
*/
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
