all : client serveur

client : libs/mySteward_client.o client/client.c
	gcc  libs/mySteward_client.o client/client.c -o client/client

serveur : libs/mySteward_serveur.o serveur/serveur.c
	gcc  libs/mySteward_serveur.o serveur/serveur.c -o serveur/serveur

mySteward_serveur.o : libs/mySteward_serveur.c headers/mySteward.h 
	gcc libs/mySteward_serveur.c -c libs/mySteward_serveur.o

mySteward_client.o : libs/mySteward_client.c headers/mySteward.h 
	gcc libs/mySteward_client.c -c libs/mySteward_client.o



clean : 
	rm -rf client/client serveur/serveur libs/*.o 