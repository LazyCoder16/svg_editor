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
    void SetScene(GraphicScene* scene);
    bool DiscardChangesDialog();
    static void ExtractShapes(const XMLTag& root, std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> >& shapes);
    void SetCurFilename(const std::string& filename) { cur_filename_ = filename; }
    std::string GetCurFilename() { return cur_filename_; }

signals:
    void SetWindowTitle(const QString& title);

public slots:
    void OpenFile();
    void NewFile();
    void SaveFile();
    void SaveAs();

private:
    XMLTag SceneToXML();
    bool SaveToFile(const std::string& filename);
    std::string OpenSaveDialog();
    GraphicScene* scene_;
    std::string cur_filename_;
};

#endif // DOCCONTROLLER_H
