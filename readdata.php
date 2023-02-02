<!DOCTYPE html>
<html><body>

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


$sql = "SELECT reading_time, Temperature, Humidity, ExTemp FROM ArduinoData ORDER BY reading_time DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>reading_time</td>  
        <td>Temprature</td> 
        <td>Humidity</td>
	<td>ExTemp</td>
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_reading_time = $row["reading_time"];
        $row_value1 = $row["Temperature"];
        $row_value2 = $row["Humidity"]; 
	$row_value3 = $row["ExTemp"]; 
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_reading_time . '</td> 
                <td>' . $row_value1 . '</td> 
                <td>' . $row_value2 . '</td>
		<td>' . $row_value3 . '</td>
              </tr>';
    }
    $result->free();
}

$conn->close();
?>
</body>
</html>
