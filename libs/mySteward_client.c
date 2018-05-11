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
    scanf("%s",barreCode); 
    int pourcentage = getPot();
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
		//printf("traiterREp a sem");
    switch(rep.code){
        case 200: //ajout ok
            strcpy(colorlcd, "green");
            print_lcd(rep.msg,colorlcd);
            break;

        case 201: //mise à la poubelle
            strcpy(colorlcd, "green");
            print_lcd(rep.msg,colorlcd);
            break;

        case 202: //alerte allergenes
            strcpy(colorlcd, "red");
            print_lcd(rep.msg,colorlcd);
            buzz();
            break;

        case 400: 
            strcpy(colorlcd, "RED");
            print_lcd(rep.msg,colorlcd);
            buzz();
            break;
		}
   CHECK( sem_post(&mutex_pot),  "sem_post error"); // on libère la mutex quand on a fini d'écrire
   //printf("traiterREp a plus sem");
	usleep(100000);
    

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
    int value=-1, oldvalue=-1, buff=-1;
    char colorlcd[10]="", msglcd[100]="";

    while(1){

        value = getPot();
   /*    
        if(0<=buff && buff<7) value=0;
       else if(7<=buff && buff<17) value=10;
       else if(17<=buff && buff<26) value=20;
       else if(26<=buff && buff<36) value=30;
       else if(36<=buff && buff<46) value=40;
       else if(46<=buff && buff<56) value=50;
       else if(56<=buff && buff<66) value=60;
       else if(66<=buff && buff<76) value=70;
       else if(76<=buff && buff<86) value=80;
       else if(86<=buff && buff<94) value=90;
       else if(94<=buff && buff<=100) value=100;
        */
        if(value != oldvalue){ //si le potentiometre change de valeur on affiche la nouvelle valeur
            oldvalue=value;

            sprintf(msglcd, "Quantite_selectionnee:_%d", value);
          //  printf("ecoute attend sem\n");
            CHECK( sem_wait(&mutex_pot), "sem_wait error");
         //  printf("ecoute a sem\n");
            print_lcd(msglcd,"blue");
            CHECK( sem_post(&mutex_pot),  "sem_post error"); 
 			//printf("ecoute a plus sem\n");
            usleep(100000);//100ms between read

        }
    }
}

int getPot(){
			int buff=-1;
			char  buffer[200]="";
	     FILE* file = popen("./modules_grovepi/modules.out getPot", "r");
        if (file == NULL)  printf("error popen\n");
        else{
            fgets(buffer, 200, file);
            if(strstr(buffer, "error") == NULL){
            	if(pclose(file) != 0)
           				printf("error closing popen stream\n");     
           			buff= atoi(buffer);

       			 if(0<=buff && buff<7) return 0;
     			  else if(7<=buff && buff<17) return 10;
    				  else if(17<=buff && buff<26) return 20;
      			else if(26<=buff && buff<36) return 30;
     			  	else if(36<=buff && buff<46) return 40;
      			 else if(46<=buff && buff<56) return 50;
      			 else if(56<=buff && buff<66) return 60;
      			 else if(66<=buff && buff<76) return 70;
      			 else if(76<=buff && buff<86) return 80;
      			 else if(86<=buff && buff<94) return 90;
       			else if(94<=buff && buff<=100) return 100;
            }
            else 	printf("%s\n", buffer );

           if(pclose(file ) != 0)
           		printf("error closing popen stream\n");
           		return -1;
        } 
}

void buzz(){
				char  buffer[200]="";
	     FILE* file = popen("./modules_grovepi/modules.out buzz", "r");
        if (file == NULL)  printf("error popen");
        else{
            fgets(buffer, 200, file);
            if(strstr(buffer, "error") != NULL)
            	printf("%s\n", buffer );
            
            if(pclose(file) != 0)
           		printf("error closing popen stream\n");
            return ;
        }
	}
	
	void print_lcd(char* msg, char* color){
				char  buffer[200]="" , cmd[500]="./modules_grovepi/modules.out print_lcd ";
				strcat(cmd, msg);
				strcat(cmd," ");
				strcat(cmd,color);
	     FILE* file = popen(cmd, "r");
        if (file == NULL)  printf("error popen");
        else{
            fgets(buffer, 200, file);
            if(strstr(buffer, "error") != NULL) //le grovepi a envoyé une erreur
            	printf("%s\n", buffer );
            	
             if(pclose(file) != 0)
           		printf("error closing popen stream\n");
            return ;
        }
	}