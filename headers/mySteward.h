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
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <math.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <time.h>
#include <pthread.h> 
#include <unistd.h> 
#include <semaphore.h> 

#define CHECK_DIF(exp,err,msg) if ((exp) != (err)) { perror(msg); exit(-1); }

#define CHECK(sts,msg) if ((sts) == -1) {perror(msg); exit(-1); }
#define CHECKp(sts,msg) if ((sts) == NULL) {perror(msg); exit(-1); }

#define MAX_BUFFER 1024
#define MAX_MSG 512
#define MAX_VERBE 10
#define POURCENTAGE 100

#define IP_SVC "0.0.0.0"
#define PORT_SVC 5000

#define MAX_CLTS 10

sem_t mutex_pot;


typedef char buffer_t[MAX_BUFFER];
typedef char msg_t[MAX_MSG];
typedef char verbe_t[MAX_VERBE];

typedef struct {
    unsigned int code;
    msg_t msg;
} protofmt_t;

typedef struct {
    unsigned int idProduct;
    buffer_t barrecode;
    buffer_t name;
    buffer_t imgUrl;
    buffer_t brand;
} product_t;

typedef void (*traiterReq)(int sock,protofmt_t req, protofmt_t* rep);
typedef void (*traiterRep)(protofmt_t rep);

typedef struct { 
    unsigned int codeReq; 
    traiterReq stmt;
} req2rep;

struct string {
  char *ptr;
  size_t len;
};

void traiterRequete(int sock,protofmt_t req, protofmt_t *rep);


int tailleREQ2REP(void);
void init_string(struct string *s);
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);
void requestApiFood(product_t * product);
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
void addBackslash(buffer_t b);
void traiterReponse( protofmt_t req);
void * ecoutePotentiometre();
int getPot();
void buzz();
void print_lcd(char* msg, char* color);


#endif