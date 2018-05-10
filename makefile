all : client serveur

client : libs/mySteward_client.o client/client.c
	gcc  libs/mySteward_client.o client/client.c -o client/client -pthread

serveur : libs/mySteward_serveur.o serveur/serveur.c
	gcc -L/usr/local/opt/openssl/lib -I/usr/local/mysql/include libs/mySteward_serveur.o serveur/serveur.c -o serveur/serveur `mysql_config --libs` `curl-config --libs` -ljson-c

libs/mySteward_serveur.o : libs/mySteward_serveur.c headers/mySteward.h 
	gcc -c -I/usr/local/mysql/include -I/usr/local/include/json-c/ libs/mySteward_serveur.c `mysql_config --cflags` `curl-config --cflags`
	mv mySteward_serveur.o libs/

libs/mySteward_client.o : libs/mySteward_client.c headers/mySteward.h 
	gcc libs/mySteward_client.c -c 
	mv mySteward_client.o libs

clean : 
	rm -rf ./client/client ./serveur/serveur libs/*.o 