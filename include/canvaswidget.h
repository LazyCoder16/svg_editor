#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QGraphicsView>
#include <QPointF>

class CanvasWidget : public QGraphicsView
{
    Q_OBJECT
public:
    CanvasWidget(QWidget *parent = nullptr);

public slots:
    void centreOn(float x, float y);

signals:
    void mousePosition(int x, int y);

protected:
    //void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    //void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPointF top_left;
};

#endif // CANVASWIDGET_H
