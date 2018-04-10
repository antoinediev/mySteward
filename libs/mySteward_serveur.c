#include "../headers/mySteward.h"

const req2rep REQ2REP [] = {
    110,traiter110
};

void traiter110(int sock,protofmt_t req, protofmt_t *rep){
    int pourcentage=0;
    buffer_t barrecode;
    sscanf(req.msg,"%d&%s",&pourcentage,barrecode);
    printf(" Code barre : %s Pourcentage : %d\n",barrecode,pourcentage);
}

void str2req(buffer_t b, protofmt_t* req){
    sscanf(b,"%u#%s",&req->code,req->msg);
}

void rep2str(protofmt_t rep,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%u#%s",rep.code,rep.msg);
}

void ecrireReponse(int sockDialogue, protofmt_t rep){
    buffer_t b;

    rep2str(rep,b);
    CHECK(write(sockDialogue,b,strlen(b)+1),"Problème envoie requete");
    printf("\tcode=\"%u\", message=\"%s\"\n",rep.code,rep.msg);
}

void lireRequete(int sockDialogue, protofmt_t *req){
    buffer_t b;

    memset(b,0,MAX_BUFFER);
    CHECK(read(sockDialogue,b,MAX_BUFFER),"Problème lecture requete");
    printf("%s\n",b);
    str2req(b,req);
    printf("\tcode : \"%u\",  msg :\"%s\"\n", req->code,req->msg);
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

void dialogueAvecClient(int sockDialogue){
    buffer_t b;
    protofmt_t req;
    protofmt_t rep;
    int nbCarLus;
    int i,len;
    
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
