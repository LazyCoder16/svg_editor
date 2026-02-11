#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QtCore/qobject.h>
#include <QtCore/qtmetamacros.h>
#include <stack>
#include <memory>
#include <QObject>
#include "commands.h"


class UndoStack : public QObject
{
    Q_OBJECT
signals:
    void StackChanged();
public:
    UndoStack();
    void AddAction(std::unique_ptr<Command> command);  // New action
    void Reset();  // Reset stacks
    void Undo();  // Undo action
    void Redo();  // Redo action
    bool IsClean();  // (counter == 0) denotes if any change has been made
    void SetClean();  // Resets counter to 0

private:
    std::stack<std::unique_ptr<Command> > undo_stack_;
    std::stack<std::unique_ptr<Command> > redo_stack_;
    int counter_;   // Counter to monitor changes
};

#endif // UNDOSTACK_H
