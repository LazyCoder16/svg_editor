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
    void stackChanged();
public:
    UndoStack();
    void addAction(std::unique_ptr<Command> command);
    void reset();
    void undo();
    void redo();
    bool isClean();
    void setClean();

private:
    std::stack<std::unique_ptr<Command> > undo_stack;
    std::stack<std::unique_ptr<Command> > redo_stack;
    int counter;
};

#endif // UNDOSTACK_H
