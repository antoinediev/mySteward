<?php

include_once "config.php";

/**
* Effectue une requete SELECT dans une base de données SQL SERVER, pour récupérer uniquement un champ (la requete ne doit donc porter que sur une valeur)
* Renvoie FALSE si pas de resultats, ou la valeur du champ sinon
* @pre Les variables  $BDD_login, $BDD_password $BDD_chaine doivent exister
* @param string $SQL
* @return false|string
*/
function SQLGetChamp($sql)
{
	global $BDD_host;
	global $BDD_base;
	global $BDD_user;
	global $BDD_password;

	mysql_connect($BDD_host,$BDD_user,$BDD_password) or die("<font color=\"red\">SQLGetChamp: Erreur de connexion : " . mysql_error() . "</font>");
	mysql_set_charset('utf8');
	mysql_select_db($BDD_base) or die ("<font color=\"red\">SQLGetChamp: Erreur select db : " . mysql_error() . "</font>");
 
	$rs = mysql_query($sql) or die("SQLGetChamp: Erreur sur la requete : <font color=\"red\">$sql</font>");
	$num = mysql_num_rows($rs);
	
	// On pourrait utiliser mysql_fetch_field() ??
	
	if ($num==0) return false;
	
	$ligne = mysql_fetch_row($rs);
	if ($ligne == false) return false;
	else return $ligne[0];

}

/**
 * Effectue une requete SELECT dans une base de données SQL SERVER
 * Renvoie FALSE si pas de resultats, ou la ressource sinon
 * @pre Les variables  $BDD_login, $BDD_password $BDD_chaine doivent exister
 * @param string $SQL
 * @return boolean|resource
 */
function SQLSelect($sql)
{	
 	global $BDD_host;
	global $BDD_base;
 	global $BDD_user;
 	global $BDD_password;
	
 	$Link = mysqli_connect($BDD_host,$BDD_user,$BDD_password) or die("<font color=\"red\">SQLSelect: Erreur de connexion : " . mysqli_error() . "</font>");
	mysqli_set_charset($Link,'utf8');
 	mysqli_select_db($Link,$BDD_base) or die ("<font color=\"red\">SQLSelect: Erreur select db : " . mysqli_error() . "</font>");

	$rs = mysqli_query($Link,$sql) or die("SQLSelect: Erreur sur la requete : <font color=\"red\">$sql" . "|" .  mysqli_error() . "</font>");
	$num = mysqli_num_rows($rs);
	if ($num==0) return false;
	else return $rs;
}

/**
*
* Parcours les enregistrements d'un résultat mysql et les renvoie sous forme de tableau associatif
* On peut ensuite l'afficher avec la fonction print_r, ou le parcourir avec foreach
* @param resultat_Mysql $result
*/
function parcoursRs($result)
{
	if  ($result == false) return array();

	while ($ligne = mysqli_fetch_assoc($result)) 
		$tab[]= $ligne;

	return $tab;
}


/* Il y a sans doute moyen de faire mieux...
	Il faut juste exécuter mysql_query("SET NAMES UTF8"); avant chaque requête... 
	Mieux d'après php.net : mysql_set_charset('utf8')
 */
function my_utf8_encode(&$val,$cle)
{
	if (is_array($val))
		$val = utf8_encode_r($val);
	else{
		$val = utf8_encode($val);
	}
}
function utf8_encode_r($tab)
{
	// On pourrait passer $tab par référence...
	array_walk ($tab, 'my_utf8_encode');
	return $tab; 
}






















?>
