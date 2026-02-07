#ifndef DOCCONTROLLER_H
#define DOCCONTROLLER_H

#include "graphicscene.h"
#include "xmlparser.h"
#include <QObject>
#include <string>
#include <QAbstractGraphicsShapeItem>
#include <QMessageBox>

class DocController : public QObject
{
    Q_OBJECT
public:
    explicit DocController(QObject *parent = nullptr);
    void setScene(GraphicScene* scene);
    bool discardChangesDialog();
    std::string curFilename;
    static void extract_shapes(const XMLTag& root, std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> >& shapes);

signals:
    void setWindowTitle(const QString& title);

public slots:
    void openFile();
    void newFile();
    void saveFile();
    void saveAs();

private:
    XMLTag sceneToXML();
    bool saveToFile(const std::string& filename);
    std::string openSaveDialog();
    GraphicScene* scene;
};

#endif // DOCCONTROLLER_H
