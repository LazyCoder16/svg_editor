#include "propertiesdock.h"
#include "graphicscene.h"
#include "shapes.h"
#include "xmlparser.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QVBoxLayout>
#include <iostream>

PropertiesDock::PropertiesDock(QWidget *parent)
    : QDockWidget(parent)
{
    form = nullptr;
}

void PropertiesDock::setScene(GraphicScene *scene)
{
    this->scene = scene;
    if(form) delete form;
    form = new ViewportPropForm(this, scene);
    this->setWidget(form);
}

void PropertiesDock::setSelectedShapes(const std::vector<QAbstractGraphicsShapeItem*>& shapes)
{
    this->selectedShapes = shapes;
    if(form) 
    {
        if(dynamic_cast<TextPropForm*>(form)) {
            auto textForm = static_cast<TextPropForm*>(form);
            // add pending changes of textEdit
            XMLTag cur = textForm->shape->toXML();
            if(cur.children[0].name != textForm->old.children[0].name) {
                textForm->addXMLAction(textForm->shape, textForm->old, cur);
            }
        }
        delete form;
        form = nullptr;
    }
    if(selectedShapes.size() == 1) {
        auto shape = selectedShapes[0];
        if(dynamic_cast<Rectangle*>(shape)) {
            form = new RectPropForm(this, scene, static_cast<Rectangle*>(shape));
        }
        else if(dynamic_cast<Circle*>(shape)) {
            form = new CirclePropForm(this, scene, static_cast<Circle*>(shape));
        }
        else if(dynamic_cast<Line*>(shape)) {
            form = new LinePropForm(this, scene, static_cast<Line*>(shape));
        }
        else if(dynamic_cast<Polygon*>(shape)) {
            form = new PolygonPropForm(this, scene, static_cast<Polygon*>(shape));
        }
        else if(dynamic_cast<FreehandPath*>(shape)) {
            form = new FreehandPropForm(this, scene, dynamic_cast<FreehandPath*>(shape));
        }
        else if(dynamic_cast<TextShape*>(shape)) {
            form = new TextPropForm(this, scene, static_cast<TextShape*>(shape));
        }
        else {
            form = new ViewportPropForm(this, scene);
        }
    }
    else {
        form = new ViewportPropForm(this, scene);
    }
    this->setWidget(form);
}
