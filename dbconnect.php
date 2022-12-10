<?php
$MyUsername = "vapo";  // enter your username for mysql
$MyPassword = "polar11!";  // enter your password for mysql
$MyHostname = "localhost";      // this is usually "localhost" unless your database resides on a different server

$dbh = mysql_pconnect($MyHostname , $MyUsername, $MyPassword);
$selected = mysql_select_db("test",$dbh);
?>