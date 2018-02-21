#include "graphicsscene.h"




GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent),
    matrix(QList< QList<square> > ())
{
    size = 40;
    x=-1;
    y=-1;
    clicked = false;
    GraphicsScene::drawBoard();
}


/* gets coordinates of clicked square */
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QPointF p = mouseEvent->scenePos();
    x= p.x()/size;
    y=p.y()/size;
    clicked = true;
    QGraphicsScene::mousePressEvent(mouseEvent);
}

int GraphicsScene::getX(){
    return x;
}

int GraphicsScene::getY(){
    return this->y;
}


/* draws board with squares */
void GraphicsScene::drawBoard(){
       QBrush blueBrush(QColor::fromRgbF(0.7, 0.7, 0.7, 1));
       QPen outlinePen(Qt::black);
       outlinePen.setWidth(2);
       for (int i = 0; i<10; i++){
           QList<square> row;
           for (int j = 0; j<10; j++){
               QGraphicsRectItem *rect = this->addRect(i*size, j*size, size, size, outlinePen, blueBrush);
               square sq;
               sq.rectangle = rect;
               sq.hidden = true;
               row.append(sq);
           }
           matrix.append(row);
       }
}

/* fills a square - ship with color depending on the ship's status */
void GraphicsScene::drawStatus(int x0, int y0, Status status){
    QGraphicsRectItem *rect = matrix.at(x0).at(y0).rectangle;
    QBrush brush(getSquareColor(status));
    rect->setBrush(brush);
}

/* gets color depending on the ship's status */
QColor GraphicsScene::getSquareColor( Status st){
    int r=0,g=0,b=0;
    switch (st) {
    case 0://sunk
        r = 200;
        g = 30;
        b = 30;
        break;
    case 1: // hit
        r = 150;
        g = 30;
        b = 30;
        break;
    case 2://miss
        r = 240;
        g = 240;
        b = 240;
        break;
    case 4: //otherchoice
        r= 20;
        g=80;
        b=90;
        break;
    case 5:
        r=255;
        g=255;
        b=60;
    default: //hidden
        r = 70;
        g = 70;
        b = 70;
        break;
    }
    QColor color(r,g,b,255);
    return color;
}

bool GraphicsScene::isHidden(int x0, int y0){
    return matrix.at(x0).at(y0).hidden;
}

void GraphicsScene::setXY(int x0, int y0){
    this->x = x0;
    this->y = y0;
}

bool GraphicsScene::getClicked() const
{
    return clicked;
}

void GraphicsScene::setClicked(bool value)
{
    clicked = value;
}
