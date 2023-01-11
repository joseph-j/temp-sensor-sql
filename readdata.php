<!DOCTYPE html>
<html><body>

<?php
require_once ("db.php");


$sql = "SELECT id, SensorName, location, Temperature, Humidity,Pressure,reading_time FROM esp32data ORDER BY id DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>SensorName</td> 
        <td>Location</td> 
        <td>Temprature</td> 
        <td>Humidity</td>
	<td>Pressure</td>
        <td>Timestamp</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_sensor = $row["SensorName"];
        $row_location = $row["location"];
        $row_value1 = $row["Temperature"];
        $row_value2 = $row["Humidity"]; 
	$row_value3 = $row["Pressure"]; 
        $row_reading_time = $row["reading_time"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_sensor . '</td> 
                <td>' . $row_location . '</td> 
                <td>' . $row_value1 . '</td> 
                <td>' . $row_value2 . '</td>
		<td>' . $row_value3 . '</td>
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?>
</body>
</html>
