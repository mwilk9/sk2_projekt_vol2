#include "widget.h"
#include <QInputDialog>
#include <QMessageBox>
using namespace std;

/* create and init widget, get user's choice about entering empty room, joinin existing room
 * start a game or exit */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget){
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeEv()));
    connect(ui->pushButton, &QPushButton::clicked, this, &Widget::end);
    yourTurn = false;
    Dialog d_new_game;
    game_end = false;
    if(d_new_game.exec()==QDialog::Accepted){
            connn();
            startGame();
    }
    else{
        exit(0);
    }
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
            if (opponentScene->isHidden(x, y)){
                if(timer->isActive()) timer->stop();
                opponentScene->drawStatus(x, y, GraphicsScene::ohterChoice);
                sendData(x,y);
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
    connect(sock,&QTcpSocket::readyRead, this, &Widget::getMsg);
    connect(sock, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &Widget::socketError);
    sock->connectToHost(server, service_port);
}



void Widget::gameOver(bool win){
    QString str;
    game_end = true;
    if (win) str = "Congratulations! Your have won the game!";
    else str= "Too bad! Your team has lost";
    sock->disconnect();
    QMessageBox mess;
    mess.setText(str);
    mess.exec();
    Dialog newg;
    newg.setMessage("Do you want to play again?");
    if (newg.exec()==QDialog::Accepted){
        connn();
        game_end = false;
        startGame();
    }
    else
        exit(0);
}

void Widget::socketError(){
    if (!game_end){
    QMessageBox mess;
    mess.setText("Cannot reach the server");
    mess.exec();
    }
    exit(0);
}

void Widget::end(){
    sock->disconnect();
    exit(0);
}



void Widget::getMsg(){
    char arr[9];
    while (sock->bytesAvailable()>=9){
        memset(arr, 0, 9);
        for (int r= 0; r<9; r++)
            sock->read(&arr[r], 1);
        int m = (int)arr[0]-48;
        int i =1;
        opponentScene->setXY(-1,-1);
        GraphicsScene *current_scene;
        switch(m){
        case 0:
            if (!yourTurn){
                current_scene = scene;
                ui->infoBoard->append("Your ship has sunk!");
            }
            else{
                current_scene = opponentScene;
                ui->infoBoard->append("Enemy ship has sunk!");
                yourTurn = false;
            }
            while (i<9 && ((int)arr[i]-48)!=10){
                current_scene->drawStatus((int)arr[i]-48, (int)arr[i+1]-48, GraphicsScene::sunk);
                i+=2;
            }
            break;
        case 1:
            if (!yourTurn){
                current_scene = scene;
                ui->infoBoard->append("Your ship has been hit!");
            }
            else{
                current_scene = opponentScene;
                ui->infoBoard->append("Enemy ship has been hit!");
                yourTurn = false;
            }
            current_scene->drawStatus((int)arr[1]-48, (int)arr[2]-48, GraphicsScene::hit);
            break;
        case 2:
            if (!yourTurn){
                current_scene = scene;
                ui->infoBoard->append("Your enemy missed!");
            }
            else{
                current_scene = opponentScene;
                ui->infoBoard->append("You missed!");
                yourTurn = false;
            }
            current_scene->drawStatus((int)arr[1]-48, (int)arr[2]-48, GraphicsScene::miss);
            break;
        case 4:
            ui->infoBoard->append("Succesfully connected!");
            while (sock->bytesAvailable()<40){
            }
            sock ->read(shipd, 40);
            drawShips();

            break;
        case 5:
            ui->infoBoard->append("Waiting for people to play...");
            break;
        case 6:
            yourTurn = true;
            ui->infoBoard->append("It's your move!");
            opponentScene->setXY(-1,-1);
            scene->setClicked(false);
            seconds = sec;
            timer->start(milisec);
            break;
        case 7:
            yourTurn = false;
            ui->infoBoard->append("It's not your move! Wait for the results!");
            break;
        case 8:
            ui->infoBoard->append("Other member chose ship at: "+QString::number((int)arr[1]-48)+" "+QString::number((int)arr[2]-48));
            current_scene = opponentScene;
            //current_scene->drawStatus((int)arr[1]-48, (int)arr[2]-48, GraphicsScene::ohterChoice);
            break;
        case 9:
            current_scene = opponentScene;
            current_scene->drawStatus((int)arr[1]-48, (int)arr[2]-48, GraphicsScene::finalChoice);
            ui->infoBoard->append("Your team chose!");
            break;
        case 11:
            gameOver(true);
        case 12:
            gameOver(false);
        default:
            qDebug("Error :(");
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
        sendData(xx, yy);
    }
    else{
        if (seconds>0){
            ui->textEdit->setText(QString::number(seconds));
            seconds--;
            timer->start(milisec);
        }
        else {
            ui->textEdit->setText(QString::number(seconds));
            ui->infoBoard->append("");
            sendData(-1,-1);

        }
        }
}




