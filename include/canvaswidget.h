#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QGraphicsView>
#include <QPointF>

/*
Inherits from QGraphicsView and displays the scene
*/
class CanvasWidget : public QGraphicsView
{
    Q_OBJECT
public:
    CanvasWidget(QWidget *parent = nullptr);

public slots:
    void CentreOn(float x, float y); // Centre on screen on x and y coordinates of the scene

signals:
    void MousePosition(int x, int y); // Send the current mouse coordinates 

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    QPointF top_left_;
};

#endif // CANVASWIDGET_H
