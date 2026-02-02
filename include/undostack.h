#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <stack>
#include <memory>
#include "commands.h"


class UndoStack
{
public:
    UndoStack();
    void addAction(std::unique_ptr<Command> command);
    void reset();
    void undo();
    void redo();

private:
    std::stack<std::unique_ptr<Command> > undo_stack;
    std::stack<std::unique_ptr<Command> > redo_stack;
};

#endif // UNDOSTACK_H
