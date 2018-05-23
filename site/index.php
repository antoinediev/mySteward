<?php
include_once "maLibSQL.php";
ini_set('display_errors','On');
?>
<html>
	<head>
		<style>
			tr{
				background-color: blanchedalmond;
			}
			td{
				text-align : center;
				vertical-align : middle;
				border : 2px solid black;
			}
			th{
				border : 2px solid black;
			}
		</style>
	</head>
	<body>
<table>
	<thead>
		<tr>
			<th> idProduct </th>
			<th> Name </th>
			<th> brand </th>
			<th> quantity </th>
			<th> img </th>
		</tr>
	</thead>
	<tbody>
<?php		
 $rep = parcoursRs(SQLSelect("SELECT * FROM PRODUCTS;"));
foreach($rep as $element){
	echo "<tr>";
	echo "<td>".$element['idPRODUCT']."</td>";
	echo "<td>".$element['name']."</td>";
	echo "<td>".$element['brand']."</td>";
	//$quantity = parcoursRs(SQLSelect("Select quantity from STOCKS where idProduct = ".$element['idPRODUCT']))
	echo "<td>".parcoursRs(SQLSelect("Select quantity from STOCKS where idProduct = ".$element['idPRODUCT']))[0]["quantity"]."</td>";
	echo "<td><img src='".$element['imgUrl']."'> </td>";
	echo "</tr>";
} 
//var_dump($rep);

// echo("test");

?>
	</tbody>
</table>
	</body>
</html>
