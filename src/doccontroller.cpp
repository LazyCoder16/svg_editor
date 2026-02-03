#include "doccontroller.h"
#include <QAbstractGraphicsShapeItem>
#include "graphicscene.h"
#include "xmlparser.h"
#include <QDebug>
#include <QGraphicsItem>
#include <memory>
#include <string>
#include <vector>
#include "shapes.h"


DocController::DocController(QObject *parent)
    : QObject{parent}
{}

void DocController::setScene(GraphicScene* scene)
{
    this->scene = scene;
}

void extract_shapes(const XMLTag& root, std::vector<std::unique_ptr<QAbstractGraphicsShapeItem> >& shapes)
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

void DocController::load_file(const QString& filename, const QString &content)
{
    qDebug() << content << "\n";
    XMLTag root(false);
    XMLParser::parse_file(content.toStdString(), root);
    std::cout << root << "\n";
    this->filename = filename.toStdString();

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
        // Transfer ownership of the created shapes to our scene
        scene->addShape(std::move(shapes[i]));
    }
}
