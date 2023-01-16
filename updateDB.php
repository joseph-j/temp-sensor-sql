<?php

echo "Connecting to Database... <br>";

//Set Connection Details
$MyUsername = "root";  // enter your username for mysql
$MyPassword = "PASSWORD";  // enter your password for mysql
$MyHostname = "localhost";      // this is usually "localhost" unless your database resides on a different server
$DB = "test";
echo "variables set...".$MyUsername.", ".$MyPassword.", ".$MyHostname.", ".$DB."<br>";

//Connect to Database
$dbh = mysqli_connect( $MyHostname, $MyUsername, $MyPassword, $DB) or die('not connecting<br>');
if (mysqli_connect_errno()) {
  echo "Failed to connect to MySQL: " . mysqli_connet_errno();
  exit();
}
echo "hooked in<br>";

//Select Database (redundant?)
$selected = mysqli_select_db($dbh, "test");

// Prepare the SQL statement
$SQL = "INSERT INTO ArduinoData (id, Temperature, Humidity) VALUES ('".$_GET["id"]."','".$_GET ["Temperature"]."','".$_GET["Humidity"]."')";
echo "constructed Statement $SQL<br>";

// Execute SQL statement
mysqli_query($dbh, $SQL);

echo "query'd push<br>";


mysqli_close($dbh);
echo "closed";

// Go to the review_data.php (optional)
//header("Location: Site_2/review_data.php");
?>
