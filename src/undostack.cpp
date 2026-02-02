#include "undostack.h"
#include "commands.h"
#include <memory>


UndoStack::UndoStack() {}

void UndoStack::addAction(std::unique_ptr<Command> action)
{
    undo_stack.push(std::move(action));
    while(!redo_stack.empty()) redo_stack.pop();
}

void UndoStack::reset()
{
    while(!undo_stack.empty()) undo_stack.pop();
    while(!redo_stack.empty()) redo_stack.pop();
}

void UndoStack::undo()
{
    if(!undo_stack.empty())
    {
        undo_stack.top()->undo();
        redo_stack.push(std::move(undo_stack.top()));
        undo_stack.pop();
    }
}

void UndoStack::redo()
{
    if(!redo_stack.empty())
    {
        redo_stack.top()->redo();
        undo_stack.push(std::move(redo_stack.top()));
        redo_stack.pop();
    }
}
