#ifndef DOCCONTROLLER_H
#define DOCCONTROLLER_H

#include <QObject>
#include <string>
#include <QAbstractGraphicsShapeItem>

class DocController : public QObject
{
    Q_OBJECT
public:
    explicit DocController(QObject *parent = nullptr);

signals:
    void addShape(QAbstractGraphicsShapeItem* shape);
    void setViewportRect(float w, float h);
    void clearScene();

public slots:
    void load_file(const QString& filename, const QString &content);

private:
    std::string filename;
};

#endif // DOCCONTROLLER_H
