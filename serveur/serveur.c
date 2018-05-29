#include "../headers/mySteward.h"


/** main **/
int main(int c, char**v){
    int sockEcoute, sockDialogue;
    pid_t pidClient;

    printf("Hello i'm mySteward your personel connected steward\n\n");
    //creation socket ecoute, association adressage et mise en ecoute
    sockEcoute = createSocketEcoute(IP_SVC,PORT_SVC);
    while(1){
       //Acceptation d'un appel 
        sockDialogue = acceptConnect(sockEcoute);
        // création fork service 
        CHECK(pidClient = fork(),"Probleme creation fork");
        if(pidClient == 0){
            //fermeture socket d'écoute
            close(sockEcoute);
            //Dialogue avec un client
            dialogueAvecClient(sockDialogue);
            //Fermeture socket dialogue 
            close(sockDialogue);
            //Fin du client
            exit(0);
        }
        //fermeture socket dialogue
        close(sockDialogue);
    }
    //fermeture socket d'écoute
    close(sockEcoute);
    return 0;
}
