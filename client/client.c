#include "../headers/mySteward.h"

int main(int c, char**v){
    int sockAppel;

    printf("Hello i'm mySteward your personel connected steward\n");
    
    //verification des parametres
    if(c<3){
        printf ("usage : %s <adrIP> <port> \n",v[0]);
		exit(-1);
    }
    //ouverture de la socket
    sockAppel = connectServer(v[1],v[2]);
    //dialogue avec le serveur via la socket 
    dialogueAvecServ(sockAppel);
    //fermeture de la socket
    close(sockAppel);
    return 0;
}