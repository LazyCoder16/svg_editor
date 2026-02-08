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
    void AddAction(std::unique_ptr<Command> command);
    void Reset();
    void Undo();
    void Redo();
    bool IsClean();
    void SetClean();

private:
    std::stack<std::unique_ptr<Command> > undo_stack_;
    std::stack<std::unique_ptr<Command> > redo_stack_;
    int counter_;
};

#endif // UNDOSTACK_H
