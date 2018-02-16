#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QList>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QBrush>
#include "qmath.h"

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QObject *parent = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    enum Status{
        sunk=0,
        hit,
        miss,
        hidden,
    };
   /* enum message{
        sunk,
        hit,
        miss,
        hidden,
        connected,
        waiting,
        yourMove,
        notYourMove,
        tmChoice

    };*/
    int getX();
    int getY();
    void drawBoard();
    QColor getSquareColor(Status status);
    void drawStatus(int x0, int y0, Status status);
    bool isHidden(int x, int y);
    void setXY(int x, int y);
    bool getClicked() const;
    void setClicked(bool value);

signals:

public slots:

private:
    struct square{
        QGraphicsRectItem* rectangle;
        bool hidden;
    };
    int x;
    int y;
    int size;
    bool clicked;
    QList < QList<square> > matrix;

};

#endif // GRAPHICSSCENE_H
