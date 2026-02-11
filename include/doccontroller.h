#ifndef DOCCONTROLLER_H
#define DOCCONTROLLER_H

#include "graphicscene.h"
#include "xmlparser.h"
#include <QObject>
#include <string>
#include <QAbstractGraphicsShapeItem>
#include <QMessageBox>

/*
Class for file handling operations
*/
class DocController : public QObject
{
    Q_OBJECT
public:
    explicit DocController(QObject *parent = nullptr);
    void SetScene(GraphicScene* scene);  // Gain access to the GraphicsScene Pointer
    bool DiscardChangesDialog();   // Displayed when closing the current file
    static void ExtractShapes(const XMLTag& root, std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> >& shapes); // Extracts the shapes given an XMLTag into shapes
    void SetCurFilename(const std::string& filename) { cur_filename_ = filename; }
    std::string GetCurFilename() { return cur_filename_; }

public slots:
    void OpenFile();
    void NewFile();
    void SaveFile();
    void SaveAs();

private:
    XMLTag SceneToXML();  // Converts the current scene to XML format for saving file
    bool SaveToFile(const std::string& filename);   // Helper to save current scene to a file
    std::string OpenSaveDialog();   // Helper to open the dialog to select a file to open; returns the filename of the file openned
    GraphicScene* scene_;
    std::string cur_filename_;
};

#endif // DOCCONTROLLER_H
