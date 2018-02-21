#include "server.h"


Server::Server(){
}


/*
 * tworzy drużyny
 * czeka na klientów i dodaje ich do odpowiednich drużyn
 * rozpoczyna rozgrywkę
*/
void Server::NewGame(){
    int count = 0;
    Team *teams[2];
    teams[0] = new Team();
    teams[1] = new Team();
    Game *game = new Game(teams);
    Team * current = teams[0];
    while (count < max_players*2){
        getNewClients(current, count, game->getPlayer((count%2))->ships);
        count++;
        current = teams[count %2];
        if (teams[0]->getSize() < max_players || teams[1]->getSize() < max_players ){
            message[0] = msg::waiting+'0';
            sendData(-1,teams[0]);
            sendData(-1,teams[1]);
        }
    }
    Play(game);
}


/*
 * czeka na ruchy drużyny
 * wybiera finałowy ruch i informuje o nim resztę drużyny
 * sprawdza wynik ruchu
 * informuje o wyniku obie drużyny
 * */
void Server::Play(Game *game){
    int i = 0;
    int j;
    int x, y;
    Game::Result res;
    Ship * ship;
    Team *tm;
    Team *tm2;
    while (true){
        ship = nullptr;
        j = (i+1)%2;
        tm = game->getPlayer(i)->t;
        tm2 = game->getPlayer(j)->t;

        /*czeka na decyzje zawodników*/
        message[0] = msg::yourMove+'0';
        sendData(-1, tm);
        waitForData(tm);
        if (checkClients(tm, tm2))
            return;

        /*wybiera ostateczny ruch*/
        ChooseSquare(x,y, tm);
        message[0] = msg::finalChoice+'0';
        message[1] = x+'0';
        message[2] = y+'0';
        sendData(-1, tm);

        /*spradza wynik ruchu*/
        res = game->makeAMove(x,y,game->getPlayer(j), ship);
        int k = 0;
        Ship::Square *sq;
        switch (res){
        case Game::Result::sunkAll:
            message[0] = msg::win+'0';
            sendData(-1, tm);
            message[0] = msg::lose+'0';
            sendData(-1, game->getPlayer(j)->t);
            return;
        case Game::Result::sunk:
                message[0]= msg::sunk+'0';
                sq = ship->getPoints();
                for (k; k< ship->getSize(); k++){
                    message[2*k+1] = sq[k].x +'0';
                    message[2*(k+1)] = sq[k].y+'0';
                }
                message[2*k+1] = msg::msgEnd+'0';
                break;
            case Game::Result::hit:
                message[0] = msg::hit+'0';
                break;
            default:
                message[0] = msg::miss+'0';
        }
        /*wysyła informacje o wyniku ruchu*/
        sendData(-1, tm);
        tm->clearAnswers();
        sendData(-1, tm2);
        i = j;
        if (!res) break;
    }
}

/*
 * Wybiera losowy ruch pośród wszystkich ruchów wybranych przez drużynę
 * */
void Server::ChooseSquare(int &x, int &y, Team * t){
    int r = rand()%t->getSize();
    client c = t->getClient(r);
    x  = c.getX();
    y  = c.getY();
}



/*
* Inicjalizacja połączenia
*/
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

/*
* Odbieranie połączeń od klientów i wysyłanie im informacji o rozmieszczeniu statków na ich planszy
*/
int Server::getNewClients(Team * t, int count, char* ships){
    sockaddr_in clientAddr{0};
    socklen_t clientAddrSize = sizeof(clientAddr);
    auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
    if(clientFd == -1) error(1, errno, "accept failed");
    client c(clientFd);
    t->addClient(c);
    nacoczekac[count].fd = clientFd;
    nacoczekac[count].events = POLLIN;
    message[0]=msg::connected+'0';
    write(clientFd, message, 9);;
    write(clientFd, ships, 40);
}

/*
 * Wysłanie informacji to dużyny
 * jeżeli fd=-1 : informacja zostaje wysłana do całej drużyny
 *  w przeciwnym wypadku informacja zostaje wysłana do całej drużyny z pominięciem clienta
 *  o deskryptorze fd
 *
 * */
void Server::sendData(int fd, Team *t){
    int cls = t->getSize();
    int cfd;
    for (int i = 0; i< cls; i++){
        client c = t->getClient(i);
        cfd = c.getFd();
        if (fd>=0){
            if (cfd != fd ){
                write(cfd, message, 9);
                printf("\nSend:");
                for (int j=0; j<9;j++)
                    printf("%d_", (int)message[j]-48);
            }
        }
        else{
            write(cfd, message, 9);
            printf("\nSend:");
            for (int j=0; j<9;j++)
                printf("%d_", (int)message[j]-48);
        }
    }
}


/*
 * Czeka na wiadomość od klienta drużyny
 * jeżeli wiadomość składa się z wartości -1,-1 - klient informuje o braku decyzji
 * inna wiadomość to wybrane pole do atakowania
 * */
int Server::waitForData(Team *t){
    int answers = 0;
    int desirableAnswers = t->getSize();
    while (answers<desirableAnswers){

        int ready = poll(nacoczekac, 5, 10);
        for (pollfd &opis : nacoczekac){
            int cfd = opis.fd;
            int cid = t->hasClient(cfd);
            if( cid>=0 ){
                int d = read(cfd, chosenOnes, 3);
                if (d==0){
                    t->removeClient(cfd);
                }
                answers++;
                int x0 = (int)chosenOnes[0]-48;
                int y0 = (int)chosenOnes[1]-48;
                if (x0>=0 && y0>=0){
                    t->setClientXY(cid, x0, y0);
                    message[0] = msg::tmChoice+'0';
                    message[1] = x0+'0';
                    message[2] = y0+'0';
                    message[3] = (int)msg::msgEnd+'0';
                    sendData(cfd, t);
                }
                else{
                    t->removeClient(cfd);
                }

            }
        }
    }
}


/*
 * wysyła informacje o wygranej drużynie t2, jeżeli w drużynie t nie ma już klientów (rozłączyli się lub nie zdecydowali przed końcem czasu
 * zwraca false jeżeli w drużynie t pozostali zawodnicy
 * */
bool Server::checkClients(Team *t, Team *t2){
    if (!t->getSize()){
        message[0] = msg::win+'0';
        sendData(-1, t2);
        return true;
    }
    return false;
}




