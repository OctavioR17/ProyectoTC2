<?php
function conectar (){
    $conexion = mysql_connect("localhost","root","embebidos");
    mysql_select_db("accesorfid", $conexion);
    mysql_query("SET NAMES 'utf8'");
}

?>