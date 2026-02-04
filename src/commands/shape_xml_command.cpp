#include "commands.h"


ShapeXMLCommand::ShapeXMLCommand(Shape* shape, const XMLTag& old, const XMLTag& cur)
    : shape(shape), old(old), cur(cur)
{
    shape->updateFromXML(cur);
}

void ShapeXMLCommand::undo()
{
    shape->updateFromXML(old);
}

void ShapeXMLCommand::redo()
{
    shape->updateFromXML(cur);
}
