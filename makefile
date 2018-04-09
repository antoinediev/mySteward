all : client serveur

client : libs/mySteward.o client/client.c
	gcc libs/mySteward.o client/client.c -o client/client

serveur : libs/mySteward.o serveur/serveur.c
	gcc libs/mySteward.o serveur/serveur.c -o serveur/serveur

mySteward.o : libs/mySteward.c headers/mySteward.h
	gcc libs/mySteward.c -c libs/mySteward.o

clean : rm client/client serveur/serveur lib/mySteward.o 