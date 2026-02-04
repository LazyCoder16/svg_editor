#ifndef DOCCONTROLLER_H
#define DOCCONTROLLER_H

#include "graphicscene.h"
#include <QObject>
#include <string>
#include <QAbstractGraphicsShapeItem>

class DocController : public QObject
{
    Q_OBJECT
public:
    explicit DocController(QObject *parent = nullptr);
    void setScene(GraphicScene* scene);

signals:
    void setWindowTitle(const QString& title);

public slots:
    void load_file(const QString& filename, const QString &content);

private:
    std::string filename;
    GraphicScene* scene;
};

#endif // DOCCONTROLLER_H
