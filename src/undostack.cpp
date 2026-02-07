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
    if(counter < 0) {
        counter = INT_MIN;
    }
    else {
        ++counter;
    }
    emit stackChanged();
}

void UndoStack::reset()
{
    while(!undo_stack.empty()) undo_stack.pop();
    while(!redo_stack.empty()) redo_stack.pop();
    setClean();
    emit stackChanged();
}

void UndoStack::undo()
{
    if(!undo_stack.empty())
    {
        undo_stack.top()->undo();
        redo_stack.push(std::move(undo_stack.top()));
        undo_stack.pop();
        if(counter != INT_MIN) {
            --counter;
        }
        emit stackChanged();
    }
}

void UndoStack::redo()
{
    if(!redo_stack.empty())
    {
        redo_stack.top()->redo();
        undo_stack.push(std::move(redo_stack.top()));
        redo_stack.pop();
        if(counter != INT_MIN) {
            ++counter;
        }
        emit stackChanged();
    }
}

bool UndoStack::isClean() {
    return counter == 0;
}

void UndoStack::setClean() {
    counter = 0;
    emit stackChanged();
}
