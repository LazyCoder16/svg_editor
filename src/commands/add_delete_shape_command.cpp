#include "commands.h"
#include "graphicscene.h"


AddDeleteShapeCommand::AddDeleteShapeCommand(std::vector<QAbstractGraphicsShapeItem*> items, GraphicScene *scene, bool added)
    : items(items), scene(scene), added(added)
{
    if(added)
    {
        for(auto item : items) scene->addShape(item);
    }
    else 
    {
        for(auto item: items) scene->deleteShape(item);
    }
}

void AddDeleteShapeCommand::undo()
{
    if(added)
    {
        for(auto item : items) scene->deleteShape(item);
    }
    else 
    {
        for(auto item : items) scene->addDeletedShape(item);
    }
}

void AddDeleteShapeCommand::redo()
{
    if(added)
    {
        for(auto item : items) scene->addDeletedShape(item);
    }
    else 
    {
        for(auto item : items) scene->deleteShape(item);
    }
}
