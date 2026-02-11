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
    // New action is added to undo and redo stack is cleared
    undo_stack_.push(std::move(action));
    while(!redo_stack_.empty()) redo_stack_.pop();
    if(counter_ < 0) {
        // If an action is added after undo then the file can never reach its original state
        counter_ = INT_MIN;
    }
    else {
        // Increment counter otherwise
        ++counter_;
    }
    emit StackChanged();  // emit signal whenever the stack changes
}

void UndoStack::Reset()
{
    // Reset the stacks and counter
    while(!undo_stack_.empty()) undo_stack_.pop();
    while(!redo_stack_.empty()) redo_stack_.pop();
    SetClean();
    emit StackChanged();
}

void UndoStack::Undo()
{
    // Pop from undo and push to redo
    if(!undo_stack_.empty())
    {
        undo_stack_.top()->Undo();
        redo_stack_.push(std::move(undo_stack_.top()));
        undo_stack_.pop();
        if(counter_ != INT_MIN) {
            // If not impossible to reach original state, decrement the counter
            --counter_;
        }
        emit StackChanged();
    }
}

void UndoStack::Redo()
{
    // Pop from redo and push to undo
    if(!redo_stack_.empty())
    {
        redo_stack_.top()->Redo();
        undo_stack_.push(std::move(redo_stack_.top()));
        redo_stack_.pop();
        if(counter_ != INT_MIN) {
            // If not impossible to reach original state, increment the counter
            ++counter_;
        }
        emit StackChanged();
    }
}

bool UndoStack::IsClean() {
    // Returns true if the file has any changes
    return counter_ == 0;
}

void UndoStack::SetClean() {
    // Resets the counter
    counter_ = 0;
    emit StackChanged();
}
