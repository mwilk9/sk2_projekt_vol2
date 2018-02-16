#include "server.h"


Server::Server(){
    //max_threads = 10;
    moveTeamA = true;

}


/*
 * create teams and add clients
 * starts a game
*/
void Server::NewGame(){

    Team *teams[2];
    teams[0] = new Team();
    teams[1] = new Team();
    int count = 0;
    Team * current = teams[0];
    Game *game = new Game(teams);
    //czekaj na klientow i ich dodaj do druzyn
    while (count < max_players*2){
        getNewClients(current, count, game->getPlayer(count)->ships);
        count++;
        current = teams[count %2];
        if (teams[0]->getSize() < max_players || teams[1]->getSize() < max_players ){
            message[0] = msg::waiting+'0';
            sendData(-1,teams[0]);
            sendData(-1,teams[1]);
        }
    }
    Play(game);
    delete teams[1];
    delete teams[0];
}


/*
 * waits for clients choices of moves
 * chooses one move
 * makes a move and inform clients about the result
 * */
void Server::Play(Game *game){
    int i = 0;
    int j;
    int x, y;
    Game::Result res;
    Ship * ship;
    Team *tm;

    //message[0] = msg::notYourMove+'0';
    //sendData(-1,game->getPlayer(1)->t);
    while (true){
        ship = nullptr;
        j = (i+1)%2;

        tm = game->getPlayer(i)->t;
        message[0] = msg::yourMove+'0';
        sendData(-1, tm);


        //message[0] = msg::notYourMove+'0';
        //sendData(-1,game->getPlayer((i+1)%2)->t);



        //czekaj na odpowiedz graczy
        waitForData(tm);

        //wybierz jeden ruch
        ChooseSquare(x,y, tm);
        //poinformuj o ostatecznie wybranym ruchu
        message[0] = msg::finalChoice+'0';
        message[1] = x+'0';
        message[2] = y+'0';
        sendData(-1, tm);
        printf("Move: %d %d\n", x,y);//testing

        res = game->makeAMove(x,y,game->getPlayer(j), ship);
        int k = 0;
        Ship::Square *sq;
        switch (res){
        case Game::Result::sunkAll:
            message[0] = msg::win+'0';
            printf("Wygrana!");
            sendData(-1, tm);
            message[0] = msg::lose+'0';
            sendData(-1, game->getPlayer(j)->t);
            return;
            //break;
        case Game::Result::sunk:
                message[0]= msg::sunk+'0';

                printf("Zatopiono statek: ");
                sq = ship->getPoints();
                for (k; k< ship->getSize(); k++){
                    message[2*k+1] = sq[k].x +'0';
                    message[2*(k+1)] = sq[k].y+'0';
                    printf("x=%d, y=%d;\t", sq[k].x, sq[k].y); //testing
                }
                printf("K=%d\n", k);
                message[2*k+1] = msg::msgEnd+'0';
                break;
            case Game::Result::hit:
                message[0] = msg::hit+'0';
                printf("Trafiono w statek! x=%d, y=%d", (int)message[1]-48, (int)message[2]-48);
                break;
            default:
                message[0] = msg::miss+'0';
                printf("Nietrafiono! x=%d, y=%d",(int)message[1]-48, (int)message[2]-48);
        }
        //i=(i+1)%2;
        game->printBoard(game->getPlayer(i)); //testing
        printf("Sending data about the result!");
        sendData(-1, tm);
        printf("Result: %d\n",res); //testing
        //game->printBoard(game->getPlayer(i)); //testing
        tm->clearAnswers();
        //i=(i+1)%2;
        sendData(-1, game->getPlayer(j)->t);
        i = j;
        //i=(i+1)%2;
        if (!res) break;

    }
}

void Server::ChooseSquare(int &x, int &y, Team * t){
    //int r = rand()%t->getSize();
    int r = 0;
    client c = t->getClient(r);
    x  = c.getX();
    y  = c.getY();
}




int Server::initCon(){
    servFd = socket(AF_INET, SOCK_STREAM, 0);
    if(servFd == -1) error(1, errno, "socket failed");
    signal(SIGPIPE, SIG_IGN);
    int res = setsockopt(servFd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if(res) error(1,errno, "setsockopt failed");
    sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
    res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if(res) error(1, errno, "bind failed");
    res = listen(servFd, 10);
    if(res) error(1, errno, "listen failed");
    setsockopt(servFd, SOL_SOCKET, TCP_NODELAY, &one, sizeof(one));
    return servFd;
}

int Server::getNewClients(Team * t, int count, char* ships){
    sockaddr_in clientAddr{0};
    socklen_t clientAddrSize = sizeof(clientAddr);
    auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
    if(clientFd == -1) error(1, errno, "accept failed");
    client c(clientFd);
    t->addClient(c);
    nacoczekac[count].fd = clientFd;
    nacoczekac[count].events = POLLIN;
    printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
    //send message about being connected
    message[0]=msg::connected+'0';
    write(clientFd, message, 9);
    printf("Wyslano do %d: ", clientFd);
    for (int k = 0; k<9; k++)
        printf("%d_", (int)message[k]-48);
    printf("\n");
    write(clientFd, ships, 40);
    printf("..");

}

int Server::sendData(int fd, Team *t){
    int cls = t->getSize();
    int cfd;
    for (int i = 0; i< cls; i++){
        client c = t->getClient(i);
        cfd = c.getFd();
        if (fd>=0){
            if (cfd != fd ){
                write(cfd, message, 9);
                printf("Wyslano: ");
                for (int k = 0; k<9; k++)
                    printf("%d_", (int)message[k]-48);
                printf("\n");
            }
        }
        else{
        write(cfd, message, 9);
        printf("Wyslano do %d: ", cfd);
        for (int k = 0; k<9; k++)
            printf("%d_", (int)message[k]-48);
        printf("\n");
        }
    }
}

int Server::waitForData(Team *t){
    int answers = 0;
    int desirableAnswers = t->getSize();
    int time = 1;
    while (answers<desirableAnswers && time){
        int ready = poll(nacoczekac, 5, 10);
        for (pollfd &opis : nacoczekac){
            int cfd = opis.fd;
            int cid = t->hasClient(cfd);
            if( cid>=0 ){
                printf("Czekam na wiadomosc od %d!\n", cfd);
                int d = read(cfd, chosenOnes, 3);
                printf("Odebrano wiadomosc od %d: przeczytana %d bajty\n", cfd,d);
                answers++;
                int x0 = (int)chosenOnes[0]-48;
                int y0 = (int)chosenOnes[1]-48;
                t->setClientXY(cid, x0, y0);
                printf("Odebrano: %d %d\n", x0, y0);
                //send a message about teammate's choice
                message[0] = msg::tmChoice+'0';
                message[1] = x0+'0';
                message[2] = y0+'0';
                message[3] = (int)msg::msgEnd+'0';
                sendData(cfd, t);

            }
        }
    }
}




