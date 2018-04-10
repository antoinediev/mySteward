#include "../headers/mySteward.h"

/**
 * Tableau contenant toutes les fonctions associées a leur code de requete
 */
const req2rep REQ2REP [] = {
    0,traiter0,
    100,traiter100,
    110,traiter110
};

/** Traitement associé aux requêtes **/
void traiter0(int sock,protofmt_t req, protofmt_t *rep){
    printf("au revoir\n");
    close(sock);
    exit(0);
}

void traiter100(int sock,protofmt_t req, protofmt_t *rep){
    if(strcmp(req.msg,"moi") == 0){
        rep->code = 200;
    } else {
        rep->code = 300;
    }
}

void traiter110(int sock,protofmt_t req, protofmt_t *rep){
    int pourcentage=0;
    buffer_t barrecode;
    sscanf(req.msg,"%d&%s",&pourcentage,barrecode);
}

/** Serialisation et deserialisation des requêtes et reponses **/
void str2rep(buffer_t b, protofmt_t* rep){
    //prend en compte les espaces dans la chaine de caractere de reponse
    memset(rep,0,sizeof(protofmt_t));
    sscanf(b,"%u#%s",&rep->code,rep->msg);
}


void req2str(protofmt_t req,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%u#%s",req.code,req.msg);
}


void str2req(buffer_t b, protofmt_t* req){
    buffer_t tmp;

    sscanf(b,"%u#%s",&req->code,req->msg);
}

void rep2str(protofmt_t rep,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%u#%s",rep.code,rep.msg);
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

void ecrireReponse(int sockDialogue, protofmt_t rep){
    buffer_t b;

    rep2str(rep,b);
    CHECK(write(sockDialogue,b,strlen(b)+1),"Problème envoie requete");
    printf("\tcode=\"%u\", message=\"%s\"\n",rep.code,rep.msg);
}

/** Lecture des requêtes et réponses **/ 
void lireRequete(int sockDialogue, protofmt_t *req){
    buffer_t b;

    memset(b,0,MAX_BUFFER);
    CHECK(read(sockDialogue,b,MAX_BUFFER),"Problème lecture requete");
    printf("%s\n",b);
    str2req(b,req);
    printf("\tcode : \"%u\",  msg :\"%s\"\n", req->code,req->msg);
}

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

int acceptConnect(int sockEcoute){
    int sock,len;
    struct sockaddr_in clt,server;
    time_t now;
    len = sizeof(clt);
    CHECK(sock=accept(sockEcoute,(struct sockaddr *)&clt, (socklen_t *)&len),"Accept\t\t[FAILED]");
    len = sizeof(server);
    CHECK(getsockname(sock,(struct sockaddr *)&server, (socklen_t *)&len),"Get socket name\t[FAILED]");
    printf("SRV[%i] : %s\tClient %s:%4i is connected with server %s:%4i using socket %i\n",getpid(),ctime(&now),inet_ntoa(clt.sin_addr),htons(clt.sin_port),inet_ntoa(server.sin_addr),htons(server.sin_port),sock);

    return sock;
}

int createSocketEcoute(char *ipSvc, int portSvc){
    struct sockaddr_in server;
    int sock,len;
    time_t now;
    //Creation de la socket 
    CHECK(sock = socket(AF_INET, SOCK_STREAM,0),"Create socket\t[FAILED]");
    //preparation adressage serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(portSvc);
    server.sin_addr.s_addr = inet_addr(ipSvc);
    bzero(server.sin_zero,8);
    //Association de l'adressage avec la socket
    CHECK(bind(sock, (struct sockaddr *)&server, sizeof server),"Bind\t\t[FAILED]");

    //Recuperation des infos de la socket 
    len = sizeof(server);
    CHECK(getsockname(sock,(struct sockaddr *)&server, (socklen_t *)&len),"Get socket name\t[FAILED]");
    printf("SRV[%i] : %s\tService is started at adress %s:%4i in socket %i\n",getpid(),ctime(&now),inet_ntoa(server.sin_addr),htons(server.sin_port),sock);
    //Mise à l'écoute de la socket 
    CHECK(listen(sock, MAX_CLTS),"Listen\t\t[FAILED]");
    return sock;
}

void connexion(int sockDialogue, protofmt_t *req){
    req->code = 100;
    memset(req->msg,0,MAX_MSG);
    printf("USERNAME : "); scanf("%s",req->msg);
    ecrireRequete(sockDialogue,*req);
}

/** user input **/
void creerRequete(protofmt_t *req){
    msg_t barreCode;
    int pourcentage = POURCENTAGE;
    scanf("%s",barreCode); 
    sprintf(req->msg,"%d&%s",pourcentage,barreCode);
    printf("%s\n",req->msg);
    req->code = 110;
    return;
}

/** Dialogue **/

void dialogueAvecServ(int sockDialogue){
    buffer_t b;
    protofmt_t req;
    protofmt_t rep;
    
    connexion(sockDialogue,&req);
    while(1){
        memset(&req,0,sizeof(protofmt_t));
        creerRequete(&req);
        ecrireRequete(sockDialogue,req);
        // Code zero pour arreter le dialogue
        if(req.code==0) break;
        lireReponse(sockDialogue,&rep);
    }  
}

void dialogueAvecClient(int sockDialogue){
    buffer_t b;
    protofmt_t req;
    protofmt_t rep;
    int nbCarLus;
    int i,len;
    while(1){
        lireRequete(sockDialogue,&req);
        if(req.code==100){traiter100(sockDialogue,req,&rep);break;}
    }
    
    while(1){
        lireRequete(sockDialogue, &req);
        for(i=0; i < sizeof(REQ2REP)/sizeof(req2rep);i++){
            if(REQ2REP[i].codeReq == req.code){
                REQ2REP[i].stmt(sockDialogue, req, &rep); // appel  traiterRequete 
                break;
            }
        }
        //traiterRequete(req,&rep);
        ecrireReponse(sockDialogue,rep);  
    }
}

