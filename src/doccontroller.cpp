#include "doccontroller.h"
#include <QAbstractGraphicsShapeItem>
#include "graphicscene.h"
#include "xmlparser.h"
#include <QDebug>
#include <QGraphicsItem>
#include <QtCore/qdebug.h>
#include <QtCore/qobject.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qmessagebox.h>
#include <memory>
#include <string>
#include <vector>
#include <QFile>
#include <sstream>
#include "shapes.h"


DocController::DocController(QObject *parent)
    : QObject{parent}
{}

void DocController::setScene(GraphicScene* scene)
{
    this->scene = scene;
}

bool DocController::discardChangesDialog()
{
    // Returns true if the file operation can continue; false if the user clicks cancel
    // Called during open file, new file and close application operations
    if(scene->undoStack.isClean()) return true; // All changes are saved
    auto reply = QMessageBox::warning(nullptr, 
                                    "Unsaved changes",
                                    "The document has been modifies.\nDo you want to save your changes?",
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if(reply == QMessageBox::Cancel) return false;
    if(reply == QMessageBox::Save) {
        if(curFilename.empty()) {
            auto filename = openSaveDialog();
            if(!filename.empty()) saveToFile(filename);
        }
        else {
            saveToFile(curFilename);
        }
        return true;
    }
    return true;
}


/*
Main slots
*/
void DocController::openFile()
{
    if(!discardChangesDialog()) return;
    QString filename = QFileDialog::getOpenFileName(
        nullptr, tr("Open File"), QString(), tr("SVG File (*.svg)"));
    if(filename.isNull()) return;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", 
                             "Cannot write file:\n" + file.errorString());
        return;
    }
    QString content = QTextStream(&file).readAll();
    qDebug() << content << "\n";
    XMLTag root(false);
    XMLParser::parse_file(content.toStdString(), root);
    std::cout << root << "\n";
    this->curFilename = filename.toStdString();

    scene->removeAllItems();
    if(root.properties.find("width") != root.properties.end() 
        && root.properties.find("height") != root.properties.end())
    {
        scene->setViewportRect(std::stof(root.properties.at("width")), std::stof(root.properties.at("height")));
    }
    std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> > shapes;
    extract_shapes(root, shapes);
    for(int i=0; i<shapes.size(); ++i) 
    {
        shapes[i]->setFlag(QGraphicsItem::ItemIsSelectable, true);
        shapes[i]->setFlag(QGraphicsItem::ItemIsMovable, true);
        scene->addShape(std::move(shapes[i]));
    }
}

void DocController::newFile()
{
    if(discardChangesDialog())
    {
        this->curFilename.clear();
        scene->removeAllItems();
        scene->undoStack.setClean();
    }
}

void DocController::saveFile()
{
    if(curFilename.empty()) {
        curFilename = openSaveDialog();
    }
    if(!curFilename.empty()) {
        if(this->saveToFile(curFilename)) {
            this->scene->undoStack.setClean();
        }
        else {
            curFilename.clear();
        }
    }
}

void DocController::saveAs()
{
    std::string filename = openSaveDialog();
    if(!filename.empty()) {
        if(this->saveToFile(filename)) {
            this->curFilename = filename;
            this->scene->undoStack.setClean();
        }
    }
}

/*
Helper for the main slots
*/
bool DocController::saveToFile(const std::string& filename) 
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", 
                             "Cannot write file:\n" + file.errorString());
        return false;
    }
    QTextStream out(&file);
    std::stringstream ss;
    ss << this->sceneToXML();
    out << QString::fromStdString(ss.str());
    file.close();
    return true;
}

std::string DocController::openSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(
        nullptr, "save file", QString(), "SVG Files (*.svg)"
    );
    return fileName.toStdString();
}

void DocController::extract_shapes(const XMLTag& root, std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> >& shapes)
{
    if(root.is_text) return;

    if(root.name == "rect") {
        shapes.push_back(std::make_unique<Rectangle>(root));
    }
    else if(root.name == "circle") {
        shapes.push_back(std::make_unique<Circle>(root));
    }
    else if(root.name == "text") {
        shapes.push_back(std::make_unique<TextShape>(root));
    }
    else if(root.name == "polygon") {
        shapes.push_back(std::make_unique<Polygon>(root));
    }
    else if(root.name == "line") {
        shapes.push_back(std::make_unique<Line>(root));
    }
    else if(root.name == "path") {
        shapes.push_back(std::make_unique<FreehandPath>(root));
    }
    for(const XMLTag& child : root.children)  extract_shapes(child, shapes);
}

XMLTag DocController::sceneToXML()
{
    XMLTag root(false);
    root.name = "svg";
    root.properties["height"] = std::to_string(scene->viewportRect->rect().height());
    root.properties["width"] = std::to_string(scene->viewportRect->rect().width());
    for(auto shape : scene->shapes) 
    {
        Shape* s = dynamic_cast<Shape*>(shape);
        root.children.push_back(s->toXML());
    }
    return root;
}
