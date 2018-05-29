#include "../headers/mySteward.h"



int main(int c, char**v){
    
    int sockAppel;

    printf("Hello i'm mySteward your personel connected steward\n");

    //verification des parametres
    if(c<3){
        printf ("usage : %s <adrIP> <port> \n",v[0]);
		exit(-1);
    }

    pthread_t threadPotentiometre;
    CHECK_DIF(pthread_create(&threadPotentiometre, NULL, ecoutePotentiometre, NULL ), 0, "erreur de créaton de threadPotentiometre"); 

    CHECK( sem_init(&mutex_pot, 0, 1),  "sem_init mutex_pot error"); 

    //ouverture de la socket
    sockAppel = connectServer(v[1],v[2]);
    //dialogue avec le serveur via la socket 
    dialogueAvecServ(sockAppel);

    CHECK_DIF(pthread_join( threadPotentiometre, NULL ), 0, "erreur de join de threadPotentiometre");

    //fermeture de la socket
    close(sockAppel);
    return 0;
}
