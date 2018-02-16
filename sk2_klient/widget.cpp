#include "widget.h"

using namespace std;

/* create and init widget, get user's choice about entering empty room, joinin existing room
 * start a game or exit */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget){
    ui->setupUi(this);
    prev_m = -1;
    connn();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeEv()));
    player = 0;
    yourTurn = false;
    startGame();
}

Widget::~Widget(){
    delete ui;
}


/* If clicked on opponent's board:
 * if chosen square is hidden send info to server and wait for the responses */
void Widget::mousePressEvent (QMouseEvent * ev){
    if (yourTurn){
    if (opponentScene->getClicked()){
        x = opponentScene->getX();
        y = opponentScene->getY();
        qDebug()<< "New x: " << x;
        qDebug()<< "New y: " << y;
        if (opponentScene->isHidden(x, y)){
            if(timer->isActive()) timer->stop();
            sendData(x,y);
            qDebug("Wyslano: x=%d, y=%d",x,y );
        }
    }
    opponentScene->setClicked(false);
    }
}


/*drawing boards and owned ships*/
void Widget::startGame(){
    scene = new GraphicsScene();
    ui->graphicsView->setScene(scene);
    opponentScene = new GraphicsScene();
    ui->graphicsView_2->setScene(opponentScene);
    x=-1;
    y=-1;
}

void Widget::drawShips(){
    for (int i = 0; i<40; i=i+2){
       scene->drawStatus((int)shipd[i]-48, (int)shipd[i+1]-48, GraphicsScene::hidden);

    }
}

void Widget::connn(){
    sock = new QTcpSocket(this);
    connect(sock, &QTcpSocket::connected, this, &Widget::connected);
    connect(sock,&QTcpSocket::readyRead, this, &Widget::getMsg);
    connect(sock, &QTcpSocket::disconnected, this, &Widget::disconnected);
    sock->connectToHost(server, service_port);
}



void Widget::disconnected(){
    qDebug()<<"Disconected :(";
    //Display info
    sock->close();

}

void Widget::gameOver(bool win){
    //show result
    //end the game
    //close sock
    //"Do you want to play again?"
}

void Widget::connected(){
    qDebug()<<"Connected! Yay!";
    //send hello
}



void Widget::getMsg(){
    char arr[9];
    qDebug()<<"Received something.. Size: "<<sock->bytesAvailable();
    while (sock->bytesAvailable()>=9){
        int r = sock->read(arr, 9);

        int m = (int)arr[0]-48;
        qDebug("Otrzymano %d bitow:", r);
        int i =1;
        opponentScene->setXY(-1,-1);
        GraphicsScene *current_scene;
        //int xx=-1;
        //int yy=-1;
        switch(m){
        case 0:
            if (!yourTurn){
                current_scene = scene;
                qDebug()<< "Drawing own scene";
            }
            else{
                current_scene = opponentScene;
                qDebug()<<"Drawing opponent scene";
                yourTurn = false;
            }
            qDebug("Sunk: \n");
            while (i<9 && ((int)arr[i]-48)!=10){
                current_scene->drawStatus((int)arr[i]-48, (int)arr[i+1]-48, GraphicsScene::sunk);

                qDebug("\tx = %d; y = %d", (int)arr[i]-48, (int)arr[i+1]-48);
                i+=2;
                //sprawdzic czy nie wychodzi za tablice
            }
            break;
        case 1:
            if (!yourTurn){
                current_scene = scene;
                qDebug()<< "Drawing own scene";
            }
            else{
                current_scene = opponentScene;
                qDebug()<<"Drawing opponent scene";
                yourTurn = false;
            }
            current_scene->drawStatus((int)arr[1]-48, (int)arr[2]-48, GraphicsScene::hit);
            qDebug("Hit: x = %d; y = %d", (int)arr[1]-48, (int)arr[2]-48);
            break;
        case 2:
            if (!yourTurn){
                current_scene = scene;
                qDebug()<< "Drawing own scene";
            }
            else{
                current_scene = opponentScene;
                qDebug()<<"Drawing opponent scene";
                yourTurn = false;
            }
            current_scene->drawStatus((int)arr[1]-48, (int)arr[2]-48, GraphicsScene::miss);
            qDebug("Miss: x = %d; y = %d\n", (int)arr[1]-48, (int)arr[2]-48);
            break;
        case 4:
            qDebug("Connected!\n");
            while (sock->bytesAvailable()<40){
                qDebug()<<"Czekam na plansze...";
            }
            qDebug()<<"Dostalam plansze";
            sock ->read(shipd, 40);
            drawShips();

            break;
        case 5:
            qDebug("Waiting for members...\n");
            break;
        case 6:
            yourTurn = true;
            qDebug("Your move...\n");
            opponentScene->setXY(-1,-1);
            //while (xx==-1 || yy==-1){
            scene->setClicked(false);
            timer->start(milisec);
            break;
        case 7:
            yourTurn = false;
            qDebug("Not your move; wait for results\n");
            break;
        case 8:
            qDebug("Member chose: x = %d; y = %d\n", (int)arr[1]-48, (int)arr[2]-48);
            break;
        case 9:
            qDebug("Final choice: x = %d; y = %d\n", (int)arr[1]-48, (int)arr[2]-48);
            break;
        case 11:
            qDebug("Win!\n");
            gameOver(true);
        case 12:
            qDebug("Loss");
            gameOver(false);
        default:
            qDebug("...");
        }

    }
}

int Widget::sendData(int xx, int yy){
    char arr[2];
    arr[0] = xx+'0';
    arr[1] = yy+'0';
    return (sock->write(arr,2));
}

void Widget::timeEv(){
    timer->stop();
    int xx= opponentScene->getX();
    int yy= opponentScene->getY();
    if (xx!=-1 && yy!=-1 && opponentScene->isHidden(xx, yy)){
        qDebug()<<"TIMEOUT Wybrano: "<<xx<<", " <<yy;
        sendData(xx, yy);
    }
    else{
        if (seconds>0){
            //qDebug()<<"Zostało " <<seconds <<" sekund!";
            seconds--;
            timer->start(milisec);
        }
        else qDebug()<<"Koniec czasu";
    }
}




