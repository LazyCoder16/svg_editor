#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include "commands.h"
#include "graphicscene.h"
#include "shapes.h"
#include "xmlparser.h"
#include <QWidget>
#include <QDockWidget>
#include <QtCore/qtmetamacros.h>
#include <QtGui/qbrush.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qspinbox.h>
#include <QtWidgets/qwidget.h>
#include <memory>
#include <vector>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>


class PropertiesForm : public QWidget
{
    Q_OBJECT;
public:
    PropertiesForm(QWidget* parent, GraphicScene* scene);
    virtual ~PropertiesForm() {}
    void addCommand(std::unique_ptr<Command> command);
    QDoubleSpinBox* getSpinBox(float init_val, bool neg=false, const std::string& suffix="px");
    QLabel* getLabel(const QString& text, int font_size=12);
    QSlider* getSlider(QBrush ival, int from=0, int to=255);
    QPushButton* getColorButton(QColor icolor);
    void addXMLAction(Shape* shape, const XMLTag& old, const XMLTag& cur);
    void implSpinBoxChange(Shape* shape, QDoubleSpinBox* spinBox, const std::string& propName);
    void implFillColor(Shape* shape, QSlider* slider, QPushButton* button);
    void implStrokeStyle(Shape* shape, QDoubleSpinBox* spinBox, QPushButton* button);
    bool blockSignals = false;

private:
    GraphicScene *scene;
    XMLTag startSliderDragState;
};


class PropertiesDock : public QDockWidget
{
    Q_OBJECT;
public:
    PropertiesDock(QWidget *parent = nullptr);
    void setScene(GraphicScene *scene);

public slots:
    void setSelectedShapes(const std::vector<QAbstractGraphicsShapeItem*>& shapes);

private:
    std::vector<QAbstractGraphicsShapeItem*> selectedShapes;
    GraphicScene *scene;
    PropertiesForm *form = nullptr;
};


class ViewportPropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    ViewportPropForm(QWidget* parent, GraphicScene* scene);
};


class RectPropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    RectPropForm(QWidget* parent, GraphicScene* scene, Rectangle* rect);
};


class CirclePropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    CirclePropForm(QWidget* parent, GraphicScene* scene, Circle* circle);
};


class LinePropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    LinePropForm(QWidget* parent, GraphicScene* scene, Line* line);
};


class PolygonPropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    PolygonPropForm(QWidget* parent, GraphicScene* scene, Polygon* polygon);
private:
    XMLTag startDragState;
    void implRadiusSlider(QSlider* slider, Polygon* polygon);
};


class FreehandPropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    FreehandPropForm(QWidget* parent, GraphicScene* scene, FreehandPath* path);
};

class TextPropForm : public PropertiesForm
{
    Q_OBJECT;
public:
    TextPropForm(QWidget* parent, GraphicScene* scene, TextShape* shape);
    ~TextPropForm();
    XMLTag old;
    TextShape* shape;
};

#endif
