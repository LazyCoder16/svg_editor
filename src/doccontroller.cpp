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

void DocController::SetScene(GraphicScene* scene)
{
    this->scene_ = scene;
}

bool DocController::DiscardChangesDialog()
{
    // Returns true if the file operation can continue; false if the user clicks cancel
    // Called during open file, new file and close application operations
    if(scene_->IsSaved()) return true; // All changes are saved
    auto reply = QMessageBox::warning(nullptr, 
                                    "Unsaved changes",
                                    "The document has been modifies.\nDo you want to save your changes?",
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if(reply == QMessageBox::Cancel) return false;
    if(reply == QMessageBox::Save) {
        if(cur_filename_.empty()) {
            auto filename = OpenSaveDialog();
            if(!filename.empty()) SaveToFile(filename);
        }
        else {
            SaveToFile(cur_filename_);
        }
        return true;
    }
    return true;
}


/*
Main slots
*/
void DocController::OpenFile()
{
    if(!DiscardChangesDialog()) return;
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
    XMLParser::ParseFile(content.toStdString(), root);
    std::cout << root << "\n";
    this->cur_filename_ = filename.toStdString();

    scene_->RemoveAllItems();
    if(root.properties.find("width") != root.properties.end() 
        && root.properties.find("height") != root.properties.end())
    {
        scene_->SetViewportRect(std::stof(root.properties.at("width")), std::stof(root.properties.at("height")));
    }
    std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> > shapes;
    ExtractShapes(root, shapes);
    for(int i=0; i<shapes.size(); ++i) 
    {
        shapes[i]->setFlag(QGraphicsItem::ItemIsSelectable, true);
        shapes[i]->setFlag(QGraphicsItem::ItemIsMovable, true);
        scene_->AddShape(std::move(shapes[i]));
    }
}

void DocController::NewFile()
{
    if(DiscardChangesDialog())
    {
        this->cur_filename_.clear();
        scene_->RemoveAllItems();
        scene_->SetSaved();
    }
}

void DocController::SaveFile()
{
    if(cur_filename_.empty()) {
        cur_filename_ = OpenSaveDialog();
    }
    if(!cur_filename_.empty()) {
        if(this->SaveToFile(cur_filename_)) {
            this->scene_->SetSaved();
        }
        else {
            cur_filename_.clear();
        }
    }
}

void DocController::SaveAs()
{
    std::string filename = OpenSaveDialog();
    if(!filename.empty()) {
        if(this->SaveToFile(filename)) {
            this->cur_filename_ = filename;
            this->scene_->SetSaved();
        }
    }
}

/*
Helper for the main slots
*/
bool DocController::SaveToFile(const std::string& filename) 
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", 
                             "Cannot write file:\n" + file.errorString());
        return false;
    }
    QTextStream out(&file);
    std::stringstream ss;
    ss << this->SceneToXML();
    out << QString::fromStdString(ss.str());
    file.close();
    return true;
}

std::string DocController::OpenSaveDialog()
{
    QString filename = QFileDialog::getSaveFileName(
        nullptr, "save file", QString(), "SVG Files (*.svg)"
    );
    return filename.toStdString();
}

void DocController::ExtractShapes(const XMLTag& root, std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> >& shapes)
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
    for(const XMLTag& child : root.children)  ExtractShapes(child, shapes);
}

XMLTag DocController::SceneToXML()
{
    XMLTag root(false);
    root.name = "svg";
    root.properties["height"] = std::to_string(scene_->GetViewPortRect()->rect().height());
    root.properties["width"] = std::to_string(scene_->GetViewPortRect()->rect().width());
    for(auto shape : scene_->GetDrawnShapes()) 
    {
        Shape* s = dynamic_cast<Shape*>(shape);
        root.children.push_back(s->ToXML());
    }
    return root;
}
