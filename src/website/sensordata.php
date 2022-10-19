<?php
	echo 'Today\'s date: ' . date("Y-m-d");
	echo "<br>";
	echo 'Time: ' . date("h:i");
	echo "<br>";
	/* Database connection settings */
    $host = 'classmysql.engr.oregonstate.edu';
    $user = 'capstone_2021_eaglea';
    $pass = 'BW8GXr.M[3tQ8)1_';
    $db = 'capstone_2021_eaglea';
	$mysqli = new mysqli($host,$user,$pass,$db) or die($mysqli->error);

	$date = date("y-m-d");
	if(isset($_POST['day1'])){
		$date = $_POST['year1'] . "-" . $_POST['month1'] . "-" . $_POST['day1'];
	} 	
	$date2 = date("y-m-d") . " 23:59:59";
	if(isset($_POST['day2'])){
		$date2 = $_POST['year2'] . "-" . $_POST['month2'] . "-" . $_POST['day2'] . " 23:59:59";
	} 	
	
	//query to get Temperature data from the table
	$sql = "SELECT * FROM `sensor_data` 
		WHERE `Sensor` = 'Temperature'";
	//start time
	$sql = $sql . "AND `Time` >= '" . $date . "'";

	//end time
	$sql = $sql . "AND `Time` <= '" . $date2 . "'";
		
    $result = mysqli_query($mysqli, $sql);

	//loop through the returned data
	while ($row = mysqli_fetch_array($result)) {
		$TempValue = $TempValue . '"'. $row['Value'] .'",';
		$TempTime = $TempTime . '"' . $row['Time'] .'",';
	}
	
	//remove last , for formatting and store in new variables
	$TempTime = trim($TempTime,",");
	$TempValue = trim($TempValue,",");

	//query to get Humidity data from the table
	$sql = "SELECT * FROM `sensor_data` 
		WHERE `Sensor` = 'Humidity'";
	//start time
	if($_POST['Time1']){
		$sql = $sql . "AND `Time` >= '" . $_POST['Time1'] . "'";
	}
	//end time
	if($_POST['Time2']){
		$sql = $sql . "AND `Time` <= '" . $_POST['Time2'] . "'";
	}
		
    $result = mysqli_query($mysqli, $sql);

	//loop through the returned data
	while ($row = mysqli_fetch_array($result)) {
		$HumValue = $HumValue . '"'. $row['Value'] .'",';
		$HumTime = $HumTime . '"' . $row['Time'] .'",';
	}
	
	//remove last , for formatting and store in new variables
	$HumTime = trim($HumTime,",");
	$HumValue = trim($HumValue,",");

	//query to get Humidity data from the table
	$sql = "SELECT * FROM `sensor_data` 
		WHERE `Sensor` = 'Pressure'";
	//start time
	if($_POST['Time1']){
		$sql = $sql . "AND `Time` >= '" . $_POST['Time1'] . "'";
	}
	//end time
	if($_POST['Time2']){
		$sql = $sql . "AND `Time` <= '" . $_POST['Time2'] . "'";
	}
		
    $result = mysqli_query($mysqli, $sql);

	//loop through the returned data
	while ($row = mysqli_fetch_array($result)) {
		$PressValue = $PressValue . '"'. $row['Value'] .'",';
		$PressTime = $PressTime . '"' . $row['Time'] .'",';
	}
	
	//remove last , for formatting and store in new variables
	$PressTime = trim($PressTime,",");
	$PressValue = trim($PressValue,",");
?>

<!-- Graph -->
	<!DOCTYPE html>
	<html>
		<head>
    		<meta name="viewport" content="width=device-width, initial-scale=1.0">
			<script type="text/javascript" src="https://cdn.jsdelivr.net/npm/chart.js@3.6.1/dist/chart.min.js"></script>
			<title>Environmental Sensor Graph</title>

			<style type="text/css">			
				body{
					font-family: Arial;
					margin: 80px 100px 10px 100px;
					padding: 0;
					color: white;
					text-align: center;
					background: #555652;
				}

				.container {
					color: #E8E9EB;
					background: #222;
					border: #555652 1px solid;
					padding: 10px;
				}
			</style>

		</head>

		<body>	   
			<div class="container">	
			<h1>Environmental Sensor Dataset</h1>       
				<canvas id="chart" style="width: 100%; height: 65vh; background: #222; border: 1px solid #555652; margin-top: 10px;"></canvas>

				<script>
					var ctx = document.getElementById("chart").getContext('2d');
    				var myChart = new Chart(ctx, {
        			type: 'line',
					data: {
						labels: [<?php echo $TempTime; ?>],
						datasets: 
						[
							//temperature data
							{
		            				label: 'Temperature',
									yAxisID: 'A',
									data: [<?php echo $TempValue; ?>],
									backgroundColor: 'transparent',
									borderColor:'rgba(0,255,255)',
									borderWidth: 3	
							},
						
							//humidity data
							{
		            				label: 'Humidity',
									yAxisID: 'B',
									data: [<?php echo $HumValue; ?>],
									backgroundColor: 'transparent',
									borderColor:'rgba(0,255,0)',
									borderWidth: 3	
							},
						
							//pressure data
							{
		            				label: 'Pressure',
									yAxisID: 'C',
									data: [<?php echo $PressValue; ?>],
									backgroundColor: 'transparent',
									borderColor:'rgba(255, 0, 0)',
									borderWidth: 3	
							},
						]
					},
		     
					options: {
						scales: {
							//temp
							A: {position: 'left', ticks: {beginAtZero: true}, title: {display: true, text: "Temperature (deg C)"}},
							//humidity
							B: {type: 'linear', position: 'right', display: true, title: {display: true, text: 'Humidity (%)'}},
							//pressure
							C: {type: 'linear', position: 'left', display: true, title: {display: true, text: 'Pressure (kPa)'}},
							xAxes: [{type: 'time', ticks: {source:'data'}, time: {unit: "minute"}}]
						},
					tooltips:{mode: 'index'},
					legend:{display: true, position: 'top', labels: {fontColor: 'rgb(255,255,255)', fontSize: 16}},
					elements: {point: {radius: 0.5}}
					
					}
				});
				</script> 
			</div>    
		</body>
	</html>
	

<!-- sensor select -->
<form action="" method="post">
	<!-- dropdown lists -->
	<br>
	<!-- Start date -->
	<label for='year1'>Year:</label>
	<select name="year1">
    <option value="<?php echo date("Y");?>"><?php echo date("Y");?></option>
    <option value="<?php echo (date("Y") - 1);?>"><?php echo (date("Y") - 1);?></option>
    <option value="<?php echo (date("Y") - 2);?>"><?php echo (date("Y") - 2);?></option>
    <option value="<?php echo (date("Y") - 3);?>"><?php echo (date("Y") - 3);?></option>
    <option value="<?php echo (date("Y") - 4);?>"><?php echo (date("Y") - 4);?></option>
	</select>

	<label for='month1'>Month:</label>
	<select name="month1">
    <option value="1">January</option>
    <option value="2">February</option>
    <option value="3">March</option>
    <option value="4">April</option>
    <option value="5">May</option>
    <option value="6">June</option>
    <option value="7">July</option>
    <option value="8">August</option>
    <option value="9">September</option>
    <option value="10">October</option>
    <option value="11">November</option>
    <option value="12">December</option>
	</select>

	<label for='day1'>Day:</label>
	<select name="day1">
    <option value="1">1</option>
    <option value="2">2</option>
    <option value="3">3</option>
    <option value="4">4</option>
    <option value="5">5</option>
    <option value="6">6</option>
    <option value="7">7</option>
    <option value="8">8</option>
    <option value="9">9</option>
    <option value="10">10</option>
    <option value="11">11</option>
    <option value="12">12</option>
    <option value="13">13</option>
    <option value="14">14</option>
    <option value="15">15</option>
    <option value="16">16</option>
    <option value="17">17</option>
    <option value="18">18</option>
    <option value="19">19</option>
    <option value="20">20</option>
    <option value="21">21</option>
    <option value="22">22</option>
    <option value="23">23</option>
    <option value="24">24</option>
    <option value="25">25</option>
    <option value="26">26</option>
    <option value="27">27</option>
    <option value="28">28</option>
    <option value="29">29</option>
    <option value="30">30</option>
    <option value="31">31</option>
	</select>
	
	<!-- To date -->
	<br><br>
	<label for='year2'>Year:</label>
	<select name="year2">
    <option value="<?php echo date("Y");?>"><?php echo date("Y");?></option>
    <option value="<?php echo (date("Y") - 1);?>"><?php echo (date("Y") - 1);?></option>
    <option value="<?php echo (date("Y") - 2);?>"><?php echo (date("Y") - 2);?></option>
    <option value="<?php echo (date("Y") - 3);?>"><?php echo (date("Y") - 3);?></option>
    <option value="<?php echo (date("Y") - 4);?>"><?php echo (date("Y") - 4);?></option>
	</select>

	<label for='month2'>Month:</label>
	<select name="month2">
    <option value="1">January</option>
    <option value="2">February</option>
    <option value="3">March</option>
    <option value="4">April</option>
    <option value="5">May</option>
    <option value="6">June</option>
    <option value="7">July</option>
    <option value="8">August</option>
    <option value="9">September</option>
    <option value="10">October</option>
    <option value="11">November</option>
    <option value="12">December</option>
	</select>

	<label for='day2'>Day:</label>
	<select name="day2">
    <option value="1">1</option>
    <option value="2">2</option>
    <option value="3">3</option>
    <option value="4">4</option>
    <option value="5">5</option>
    <option value="6">6</option>
    <option value="7">7</option>
    <option value="8">8</option>
    <option value="9">9</option>
    <option value="10">10</option>
    <option value="11">11</option>
    <option value="12">12</option>
    <option value="13">13</option>
    <option value="14">14</option>
    <option value="15">15</option>
    <option value="16">16</option>
    <option value="17">17</option>
    <option value="18">18</option>
    <option value="19">19</option>
    <option value="20">20</option>
    <option value="21">21</option>
    <option value="22">22</option>
    <option value="23">23</option>
    <option value="24">24</option>
    <option value="25">25</option>
    <option value="26">26</option>
    <option value="27">27</option>
    <option value="28">28</option>
    <option value="29">29</option>
    <option value="30">30</option>
    <option value="31">31</option>
</select>
	<input type="submit" name="submit" value="Submit"> 
	
</form>

<?php
	//link to graph 
	echo '<a href="http://web.engr.oregonstate.edu/~eaglea/sensordata.php">Live Graph of Today\'s Data</a>';
?>



<?php
//refresh page
$page = $_SERVER['PHP_SELF'];
//refresh page every 30sec if date is not specified. 1 hr if specified
$sec = "30";
if(isset($_POST['day1'])){
	$sec = "3600";
}
?>
<html>
    <head>
    <meta http-equiv="refresh" content="<?php echo $sec?>;URL='<?php echo $page?>'">
    </head>
</html>