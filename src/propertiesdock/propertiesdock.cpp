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
    form_ = nullptr;
}

void PropertiesDock::SetScene(GraphicScene *scene)
{
    this->scene_ = scene;
    // Reset the current form
    if(form_) delete form_;
    form_ = new ViewportPropForm(this, scene);
    this->setWidget(form_);
}

void PropertiesDock::SetSelectedShapes(const std::vector<QAbstractGraphicsShapeItem*>& shapes)
{
    // Is called whenever the selection is changed
    this->selected_shapes_ = shapes;
    if(form_) 
    {
        // Save pending changes of the text edit in TextPropForm
        if(dynamic_cast<TextPropForm*>(form_)) {
            auto text_form = static_cast<TextPropForm*>(form_);
            text_form->SaveCurChanges();
        }
        // Delete the current form
        delete form_;
        form_ = nullptr;
    }
    if(selected_shapes_.size() == 1) {
        auto qshape = selected_shapes_[0];
        // Dynamic cast is needed to safely translate it to a Shape* (multiple inheritence)
        if(auto shape = dynamic_cast<Shape*>(qshape)) {
            form_ = shape->GetPropertyForm(this);
        }
        else {
            form_ = new ViewportPropForm(this, scene_);
        }
    }
    else {
        form_ = new ViewportPropForm(this, scene_);
    }
    this->setWidget(form_);  // Set the new form
}
