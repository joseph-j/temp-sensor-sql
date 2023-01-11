// Prepare the SQL statement
$SQL = "INSERT INTO test.newdata (id ,Temperature ,Humidity) VALUES ('".$_GET["id"]."','".$_GET ["Temperature"]."','".$_GET["Humidity"]."')";   
//$SQL1 = "INSERT INTO test.room (room) VALUES ('".$_GET["room"]."')";              

// Execute SQL statement
mysql_query($SQL);
//mysql_query($SQL1);
//nclude("select_room.php");


// Go to the review_data.php (optional)
//header("Location: Site_2/review_data.php");
?>` 