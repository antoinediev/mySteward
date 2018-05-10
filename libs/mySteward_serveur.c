#include "../headers/mySteward.h"
#include <mysql/my_global.h>
#include <mysql/mysql.h>

const req2rep REQ2REP [] = {
    110,traiter110
};

void traiter110(int sock,protofmt_t req, protofmt_t *rep){
    int pourcentage=0;
    buffer_t barrecode;
    buffer_t query;
    int productInDB=0;
    int num_fields;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_ROW rowProduct;

    char *server = "localhost";
    char *user = "root";
    char *password = "raspberry"; 
    char *database = "mySteward";

    rep->code = 500;
    strcpy(rep->msg,"errorTraitement");

    sscanf(req.msg,"%d&%s",&pourcentage,barrecode);
    memset(query,0,MAX_BUFFER);
    sprintf(query,"SELECT * FROM PRODUCTS WHERE barrecode = %s",barrecode);
    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
    }
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    //res = mysql_use_result(conn);
    res = mysql_store_result(conn);

    num_fields = mysql_num_fields(res);

    while((row = mysql_fetch_row(res)) != NULL){
        productInDB = 1;
        rowProduct = row;
    }
    //Le produit est deja present dans la base
    if(productInDB){
        if(pourcentage==100){ // pourcentage égale à 100
            printf("pourcentage = 100\n");
            memset(query,0,MAX_BUFFER);
            sprintf(query,"update STOCKS set quantity = quantity + 1 where idProduct = %s ",rowProduct[0]);

            if (mysql_query(conn, query)) {
                fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                exit(1);
            }
           
        } else if(pourcentage <100 && pourcentage>0){ //Pourcentage different de 100
            printf("pourcentage != 100\n");
            double actualQuantity;
            memset(query,0,MAX_BUFFER);
            sprintf(query,"select quantity from STOCKS where idProduct =  %s ",rowProduct[0]);
            if (mysql_query(conn, query)) {
                fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                exit(1);
            }
            res = mysql_store_result(conn);
            num_fields = mysql_num_fields(res);
            while((row = mysql_fetch_row(res)) != NULL){
                sscanf(row[0],"%lf",&actualQuantity);
            }
            if(floor(actualQuantity) == actualQuantity){//si la quantité en base est un entier
                printf("quantite en base entier\n");
                memset(query,0,MAX_BUFFER);
                sprintf(query,"update STOCKS set quantity = quantity - 1 + %lf where idProduct = %s ",(double)pourcentage/100, rowProduct[0]);
                if (mysql_query(conn, query)) {
                    fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                    exit(1);
                }
            } else {//quantité en base est un decimal 
                memset(query,0,MAX_BUFFER);
                sprintf(query,"update STOCKS set quantity = %lf where idProduct = %s ",(floor(actualQuantity)+(double)pourcentage/100), rowProduct[0]);
                if (mysql_query(conn, query)) {
                    fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                    exit(1);
                }
            }
        } else { //Pourcentage = 0
            double actualQuantity;
            memset(query,0,MAX_BUFFER);
            sprintf(query,"select quantity from STOCKS where idProduct =  %s ",rowProduct[0]);
            if (mysql_query(conn, query)) {
                fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                exit(1);
            }
            res = mysql_store_result(conn);
            num_fields = mysql_num_fields(res);
            while((row = mysql_fetch_row(res)) != NULL){
                sscanf(row[0],"%lf",&actualQuantity);
            }
            if(floor(actualQuantity) == actualQuantity){//si la quantité en base est un entier
                printf("quantite en base entier\n");
                memset(query,0,MAX_BUFFER);
                sprintf(query,"update STOCKS set quantity = %lf where idProduct = %s ", actualQuantity-1.0, rowProduct[0]);
                if (mysql_query(conn, query)) {
                    fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                    exit(1);
                }
            } else {//quantité en base est un decimal 
                memset(query,0,MAX_BUFFER);
                sprintf(query,"update STOCKS set quantity = %lf where idProduct = %s ",floor(actualQuantity), rowProduct[0]);
                if (mysql_query(conn, query)) {
                    fprintf(stderr, "[Error]%s\n", mysql_error(conn));
                    exit(1);
                }
            }
        }
    } else { // le produit n'est pas présent il faut l'ajouter 
        printf("Produit non present dans la base\n");
        product_t product;
        strcpy(product.barrecode,barrecode);
        requestApiFood(&product);
        memset(query,0,MAX_BUFFER);
        sprintf(query,"insert into PRODUCTS (`barrecode`,`name`,`imgUrl`,`brand`) values ('%s','%s','%s','%s');",product.barrecode,product.name,product.imgUrl,product.brand);
        addBackslash(query);
        if (mysql_query(conn, query)) {
            fprintf(stderr, "[Error]%s\n", mysql_error(conn));
            exit(1);
        }
        memset(query,0,MAX_BUFFER);
        sprintf(query,"insert into STOCKS (`quantity`,`idProduct`) values ('%lf',(select idPRODUCT from PRODUCTS where barrecode = %s));",(double)pourcentage/100,barrecode);
        if (mysql_query(conn, query)) {
            fprintf(stderr, "[Error]%s\n", mysql_error(conn));
            exit(1);
        }       
    }
    
    mysql_free_result(res);
    mysql_close(conn);
    rep->code = 200;
    strcpy(rep->msg,"ok");
}

void requestApiFood(product_t* product){
    CURL *curl;
    CURLcode res;
    struct string s;
    struct json_object *jobj;
    struct json_object *jobj2;
    struct json_object *jobjBrand;
    struct json_object *jobjName;
    struct json_object *jobjImgUrl;
    buffer_t url;
      /* In windows, this will init the winsock stuff */ 
    curl_global_init(CURL_GLOBAL_ALL);
    sprintf(url,"http://fr.openfoodfacts.org/api/v0/produit/%s.json",product->barrecode);
    /* get a curl handle */ 
    curl = curl_easy_init();
    if(curl) {
        init_string(&s);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        //json part
        jobj = json_tokener_parse(s.ptr);
        json_object_object_get_ex(jobj,"product",&jobj2);
        json_object_object_get_ex(jobj2,"brands",&jobjBrand);
        json_object_object_get_ex(jobj2,"product_name_fr",&jobjName);
        json_object_object_get_ex(jobj2,"image_url",&jobjImgUrl);

        strcpy(product->brand , json_object_get_string(jobjBrand));
        strcpy(product->name , json_object_get_string(jobjName));
        strcpy(product->imgUrl , json_object_get_string(jobjImgUrl));
        printf("product : {\n\tname : \"%s\",\n\tbrand : \"%s\",\n\t imgUrl : \"%s\"\n}\n ",product->name,product->brand,product->imgUrl);

        free(s.ptr);
        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
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
    CHECK(bind(sock, (struct sockaddr *)&server, sizeof server),"Bind\t[FAILED]");

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

void addBackslash(buffer_t b){
    int i,j;
    buffer_t buff;
    for(i=0;i<strlen(b);i++){
        if( b[i]==34 || b[i] == 39 ){
            for(j=0; j<i;j++){
                buff[j]=b[j];
            }
            strcat(buff,"\\");
            for(j=i+1;j<strlen(b)+1;j++){
                buff[j]=b[j-1];
            }
            memset(b,0,1024);
            strcpy(b,buff);
            memset(buff,0,1024);
            i++;    
        } 
    }
}
