#include <iostream>
#include "server.h"
#include <signal.h>
#include <stdlib.h>
#include <time.h>
using namespace std;
//int MAX_THREADS = 10;
int servFd;

static void finish(int ){
    close(servFd);
    printf("\nClosing the app...\n");
    exit(0);
}



int main(){
    srand (time(NULL));
    Server *s = new Server();

    servFd = s->initCon();
    signal(SIGINT, finish);
    while (true){
        s->NewGame();
    }


    return 0;
}
