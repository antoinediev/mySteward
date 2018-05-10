#!/bin/bash

#suis-je root?
if [ `id -u` -ne 0 ]
then
	echo "Vous devez être root."
	echo "Faite un man sudo pour voir le manuel."
	exit 1
fi

echo "Sommes-nous sur le serveur ? [y-N]"
read reponse
if [ -n "$reponse" ]
then
	if [ $reponse == "y" ]
	then
		sudo apt-get --assume-yes install libcurl4-openssl-dev libmariadbclient-dev libjson-c-dev mysql-server php-mysql
        make
        sudo mysql --user=root < database/installDB.sql
        mysql -u root < database/mySteward.sql 
	else
		sudo apt-get --assume-yes install libcurl4-openssl-dev libmariadbclient-dev libjson-c-dev mysql-server php-mysql
        make
	fi
fi

