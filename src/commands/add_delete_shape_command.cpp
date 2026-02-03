#include "commands.h"
#include "graphicscene.h"


AddDeleteShapeCommand::AddDeleteShapeCommand(std::vector<QAbstractGraphicsShapeItem*> items, GraphicScene *scene, bool added)
    : items(items), scene(scene), added(added)
{
    // Shape add command is called by the scene before initializing this command as the scene owns the shape
    if(!added) 
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
