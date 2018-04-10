#ifndef mysteward_h
#define mysteward_h

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <time.h>
#include <netdb.h>

#define CHECK(sts,msg) if ((sts) == -1) {perror(msg); exit(-1); }
#define CHECKp(sts,msg) if ((sts) == NULL) {perror(msg); exit(-1); }

#define MAX_BUFFER 1024
#define MAX_MSG 512
#define MAX_VERBE 10
#define POURCENTAGE 100

#define IP_SVC "0.0.0.0"
#define PORT_SVC 5000

#define MAX_CLTS 10

typedef char buffer_t[MAX_BUFFER];
typedef char msg_t[MAX_MSG];
typedef char verbe_t[MAX_VERBE];

typedef struct {
    unsigned int code;
    msg_t msg;
} protofmt_t;

typedef void (*traiterReq)(int sock,protofmt_t req, protofmt_t* rep);
typedef void (*traiterRep)(protofmt_t rep);

typedef struct { 
    unsigned int codeReq; 
    traiterReq stmt;
} req2rep;

int tailleREQ2REP(void);

void traiter110(int sock,protofmt_t req, protofmt_t *rep);
void traiter100(int sock,protofmt_t req, protofmt_t *rep);
void traiter0(int sock,protofmt_t req, protofmt_t *rep);

void str2rep(buffer_t b, protofmt_t* rep);
void req2str(protofmt_t req,buffer_t b);
void str2req(buffer_t b, protofmt_t* req);
void rep2str(protofmt_t rep,buffer_t b);
void ecrireRequete(int sockDialogue, protofmt_t req);
void ecrireReponse(int sockDialogue, protofmt_t rep);
void lireRequete(int sockDialogue, protofmt_t *req);
void lireReponse(int sockDialogue, protofmt_t *req);
int connectServer(char *hostAddr, char *portNum);
int acceptConnect(int sockEcoute);
int createSocketEcoute(char *ipSvc, int portSvc);
void creerRequete(protofmt_t *req);
void dialogueAvecServ(int sockDialogue);
void dialogueAvecClient(int sockDialogue);


#endif