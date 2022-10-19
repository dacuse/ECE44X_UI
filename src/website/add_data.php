<h1> Environmental Sensor Data Input </h1>

<!DOCTYPE html>
<html>
	<head>
    	<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.2/Chart.bundle.min.js"></script>
		<title>Environmental Sensor Data Input</title>

		<style type="text/css">			
			body{
				font-family: Arial;
			    margin: 80px 100px 10px 100px;
			    padding: 0;
			    color: white;
			    text-align: center;
			    background: #778899;
			}

			.container {
				color: #E8E9EB;
				background: #222;
				border: #555652 1px solid;
				padding: 10px;
			}
		</style>

	</head>

<?php
    // Connect to MySQL
	$servername = 'classmysql.engr.oregonstate.edu';
	$username = 'capstone_2021_eaglea';
	$password = 'BW8GXr.M[3tQ8)1_';
	$dbname = 'capstone_2021_eaglea';
	
	$conn = mysqli_connect($servername, $username, $password, $dbname);

	// Create database --------------------------------------------------------
	$sql = "CREATE DATABASE IF NOT EXISTS db";

	$dbname = 'db';
	mysqli_select_db ( $conn , $dbname);

	if (!$conn) {
	    die("select sensor_data connection failed: " . mysqli_connect_error());
		echo "not connected";
	}

	//create table --------------------------------------------------
	$sql = "CREATE TABLE IF NOT EXISTS `test_sensor_data` (
	  `Sensor` VARCHAR(50) NOT NULL,
	  `Value` VARCHAR(50) NOT NULL,
	  `Time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
	  `ID` INT NOT NULL AUTO_INCREMENT,
	  PRIMARY KEY (`ID`))";

	if ($conn->query($sql) === TRUE) {
	  echo "Table created successfully  ";
	} else {
	  echo "Error creating table: " . $conn->error;
	}

    // Prepare the SQL statement
    date_default_timezone_set('America/Los_Angeles');
    $dateS = date('m/d/Y h:i:s', time());
    echo $dateS;

	echo ' <a href="http://web.engr.oregonstate.edu/~eaglea/sensordata.php">Graph</a>';

	$SQL = "INSERT INTO capstone_2021_eaglea.weather_data (datetime, temp, humidity, pressure) VALUES ('$dateS','".$_GET["temp"]."','".$_GET["hum"]."','".$_GET["pr"]."')";
    // Execute SQL statement
    mysql_query($SQL);
	
	mysqli_close($conn);

    //link to graph webpage
	echo "webpage";


	/* Insert a lot of garbage test values
	if($_POST['Sensor'] && $_POST['Value']){
		echo "Inserted new data --> Sensor: " . $_POST['Sensor'];
		echo ", Value: " . $_POST['Value'] . "<br>";
		//echo "INSERT INTO sensor_data (Sensor, Value) VALUES
		//('{$_POST['Sensor']}', '{$_POST['Value']}')";
		$query = "INSERT INTO sensor_data (Sensor, Value) VALUES
		('{$_POST['Sensor']}', '{$_POST['Value']}')";
		$conn->query($query);
	}

	//insert a lot of data for testing
	for($d = 0; $d < 32; $d++){
		for($i = 0; $i < 24; $i++){
			for($m = 0; $m < 60; $m+=15){
				$temp = rand(0, 100);
				$query = "INSERT INTO sensor_data (Sensor, Value, Time) VALUES
				('Temperature', '{$temp}', '2022-01-{$d} {$i}:{$m}:00')";
				$conn->query($query);	
			
				$hum = rand(0, 100);
				$query = "INSERT INTO sensor_data (Sensor, Value, Time) VALUES
				('Humidity', '{$hum}', '2022-01-{$d} {$i}:{$m}:00')";
				$conn->query($query);	

				$pressure = rand(0, 100);
				$query = "INSERT INTO sensor_data (Sensor, Value, Time) VALUES
				('Pressure', '{$pressure}', '2022-01-{$d} {$i}:{$m}:00')";
				$conn->query($query);	
			}
		}		
	}
		*/
?>