#include "undostack.h"
#include "commands.h"
#include <QtCore/qtmetamacros.h>
#include <climits>
#include <memory>


UndoStack::UndoStack() {
    setClean();
}

void UndoStack::addAction(std::unique_ptr<Command> action)
{
    undo_stack.push(std::move(action));
    while(!redo_stack.empty()) redo_stack.pop();
    emit stackChanged();
    if(counter < 0) {
        counter = INT_MIN;
    }
    else {
        ++counter;
    }
}

void UndoStack::reset()
{
    while(!undo_stack.empty()) undo_stack.pop();
    while(!redo_stack.empty()) redo_stack.pop();
    emit stackChanged();
    setClean();
}

void UndoStack::undo()
{
    if(!undo_stack.empty())
    {
        undo_stack.top()->undo();
        redo_stack.push(std::move(undo_stack.top()));
        undo_stack.pop();
    }
    emit stackChanged();
    if(counter != INT_MIN) {
        --counter;
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
    emit stackChanged();
    if(counter != INT_MIN) {
        ++counter;
    }
}

bool UndoStack::isClean() {
    return counter == 0;
}

void UndoStack::setClean() {
    counter = 0;
}
