#include "undostack.h"
#include "commands.h"
#include <QtCore/qtmetamacros.h>
#include <climits>
#include <memory>


UndoStack::UndoStack() {
    SetClean();
}

void UndoStack::AddAction(std::unique_ptr<Command> action)
{
    undo_stack_.push(std::move(action));
    while(!redo_stack_.empty()) redo_stack_.pop();
    if(counter_ < 0) {
        counter_ = INT_MIN;
    }
    else {
        ++counter_;
    }
    emit StackChanged();
}

void UndoStack::Reset()
{
    while(!undo_stack_.empty()) undo_stack_.pop();
    while(!redo_stack_.empty()) redo_stack_.pop();
    SetClean();
    emit StackChanged();
}

void UndoStack::Undo()
{
    if(!undo_stack_.empty())
    {
        undo_stack_.top()->Undo();
        redo_stack_.push(std::move(undo_stack_.top()));
        undo_stack_.pop();
        if(counter_ != INT_MIN) {
            --counter_;
        }
        emit StackChanged();
    }
}

void UndoStack::Redo()
{
    if(!redo_stack_.empty())
    {
        redo_stack_.top()->Redo();
        undo_stack_.push(std::move(redo_stack_.top()));
        redo_stack_.pop();
        if(counter_ != INT_MIN) {
            ++counter_;
        }
        emit StackChanged();
    }
}

bool UndoStack::IsClean() {
    return counter_ == 0;
}

void UndoStack::SetClean() {
    counter_ = 0;
    emit StackChanged();
}
