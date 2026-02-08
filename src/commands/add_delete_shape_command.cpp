#include "commands.h"
#include "graphicscene.h"


AddDeleteShapeCommand::AddDeleteShapeCommand(std::vector<QAbstractGraphicsShapeItem*> items, GraphicScene *scene, bool added)
    : items_(items), scene_(scene), added_(added)
{
    // Shape add command is called by the scene before initializing this command as the scene owns the shape
    if(!added) 
    {
        for(auto item: items) scene->DeleteShape(item);
    }
}

void AddDeleteShapeCommand::Undo()
{
    if(added_)
    {
        for(auto item : items_) scene_->DeleteShape(item);
    }
    else 
    {
        for(auto item : items_) scene_->AddDeletedShape(item);
    }
}

void AddDeleteShapeCommand::Redo()
{
    if(added_)
    {
        for(auto item : items_) scene_->AddDeletedShape(item);
    }
    else 
    {
        for(auto item : items_) scene_->DeleteShape(item);
    }
}
