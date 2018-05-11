#include "../headers/mySteward.h"

extern const req2rep REQ2REP [];

void req2str(protofmt_t req,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%u#%s",req.code,req.msg);
}

void str2rep(buffer_t b, protofmt_t* rep){
    //prend en compte les espaces dans la chaine de caractere de reponse
    memset(rep,0,sizeof(protofmt_t));
    sscanf(b,"%u#%s",&rep->code,rep->msg);
}

/**
 * Envoie des requêtes et réponses via la socket de dialogue
 */
void ecrireRequete(int sockDialogue, protofmt_t req){
    buffer_t b;

    req2str(req,b);
    CHECK(write(sockDialogue,b,strlen(b)+1),"Problème envoie requete");
    printf("\tcode=\"%u\", message=\"%s\"\n",req.code,req.msg);

}

/** Lecture des requêtes et réponses **/ 
void lireReponse(int sockDialogue, protofmt_t *req){
    buffer_t b;

    memset(b,0,MAX_BUFFER);
    CHECK(read(sockDialogue,b,MAX_BUFFER),"Problème lecture requete");
    str2rep(b,req);
    printf("\tcode:#%u#, msg=#%s#\n", req->code,req->msg);
}

/** Connexion **/
int connectServer(char *hostAddr, char *portNum){
    struct hostent *host;
    buffer_t msgUsage;
    int sock,len;
    struct sockaddr_in serv, myAddr;
    time_t now;
    //Resolution DNS par adresse 
    len = sizeof(myAddr);
    //Creation socket
    CHECK(sock=socket(AF_INET,SOCK_STREAM,0),"Can't create socket");
    //Preparation serveur
    serv.sin_family = AF_INET;
    serv.sin_port = htons(atoi(portNum));
    
    //bcopy(host->h_addr,(char *) &serv.sin_addr, host->h_length);
    
    serv.sin_addr.s_addr = inet_addr(hostAddr);
    //serv.sin_addr.s_addr = inet_addr(host->h_addr);

    bzero(serv.sin_zero,8); //ie : memset(serv.sin_zero,8,0)
    //Connexion au serveur 
    len = sizeof(serv);
    CHECK(connect(sock, (struct sockaddr *)&serv,len),"Can't connect");
    //Quel est mon adressage ? 
    len = sizeof(myAddr);
    CHECK(getsockname(sock, (struct sockaddr *)&myAddr,(socklen_t *)&len),"Can't get socket name");
    CHECK(time(&now),"Get time\t[FAILED]");
    printf("client[%i]: %s\n\tUsed address by the client is %s\n\tAllocated port for the client is %i\n",getpid(),ctime(&now),inet_ntoa(myAddr.sin_addr),ntohs(myAddr.sin_port));
    return sock;
}


/** user input **/
void creerRequete(protofmt_t *req){
    msg_t barreCode;
    int pourcentage = POURCENTAGE;
    //int pourcentage = read_potentiometre();
    scanf("%s",barreCode); 
    sprintf(req->msg,"%d&%s",pourcentage,barreCode);
    printf("%s\n",req->msg);
    req->code = 110;
    return;
}

void traiterReponse( protofmt_t rep ){
    int pourcentage=0;
    buffer_t code;
    char colorlcd[10];
    CHECK( sem_wait(&mutex_pot), "sem_wait error"); //on attend la mutex pour ecrire sur le LCD

    switch(rep.code){
        case 200: //ajout ok
            strcpy(colorlcd, "green");
         //   print_lcd(rep.msg,colorlcd);
            break;

        case 201: //mise à la poubelle
            strcpy(colorlcd, "green");
            //print_lcd(rep.msg,colorlcd);
            break;

        case 202: //alerte allergenes
            strcpy(colorlcd, "green");
         //   print_lcd(rep.msg,colorlcd);
            //buzz();
            break;

        case 400: 
            strcpy(colorlcd, "RED");
         //   print_lcd(rep.msg,colorlcd);
            //buzz();
            break;

    sleep(3);
    strcpy(colorlcd, "black");
 //   print_lcd("",colorlcd);
    CHECK( sem_post(&mutex_pot),  "sem_post error"); // on libère la mutex quand on a fini d'écrire

    }

}
/** Dialogue **/

void dialogueAvecServ(int sockDialogue){
    buffer_t b;
    protofmt_t req;
    protofmt_t rep;
    
    while(1){
        memset(&req,0,sizeof(protofmt_t));
        creerRequete(&req);
        ecrireRequete(sockDialogue,req);
        
        lireReponse(sockDialogue,&rep);
        traiterReponse(rep);
    }  
}


void * ecoutePotentiometre(){
    printf("Ecoute du potentiomètre lancée\n");
    int value=-1, oldvalue=-1;
    char colorlcd[10]="", msglcd[10]="", buffer[200]="";

    while(1){
        FILE* file = popen("../modules_grovepi/modules.out getPot", "r");
        if (file == NULL)  printf("error popen");
        else{
            fgets(buffer, 200, file);
            if(strstr(buffer, "error") == NULL){
                value = atoi(buffer);
            }
            else printf("%s\n", buffer );
        }
        //value = read_potentiometre();
        if(value != oldvalue){ //si le potentiometre change de valeur on affiche la nouvelle valeur
            oldvalue=value;

            strcpy(colorlcd, "blue");
            sprintf(msglcd, "Quantité selectionnée: %d", value);
            CHECK( sem_wait(&mutex_pot), "sem_wait error");
         //   print_lcd(msglcd,colorlcd);
            CHECK( sem_post(&mutex_pot),  "sem_post error");

            usleep(100000);//100ms between read

        }
    }
}
