import processing.serial.*;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.io.UnsupportedEncodingException;

Serial myPort;   //the serial port

String val = "";    //declare variables
String url = "";

//setup and connect to serial port
void setup(){
  println("Available serial ports");
  println(Serial.list());
  String portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
  delay(1000);
  read();
}

//reads serial data
void read(){
  while (true) {
    
    while(!(myPort.available() > 0)) { //wait until the port is available
      delay(10);
    }
  
    val = myPort.readStringUntil('\n');  //read from serial and store in val
    val = val.replace("\n", "");
    val = val.replace(" ", "");

    println(val);
    if(val.length() < 55){            //if error is read from the microcontroller open the error page
      link("https://web.engr.oregonstate.edu/~eaglea/error.php");
    }
    
    else{                            //else open the page to store microcontroller data
      link(val.substring(0, 82));
      //String param;
      //param = String.valueOf(val.toCharArray(), 54, val.length()-54); 
      //param =  URLEncoder.encode(param); 
      //url = String.valueOf(val.toCharArray(), 0, 54) + param;
      //url = url.replace("\n", "");
      //url = url.replace("%0D", "");
    }
      
    //println(url); //print the url to console
    //link(url);    //open url link (this will run the php page and upload data to the database)
    val = null;   //clear the value
  } 
}
