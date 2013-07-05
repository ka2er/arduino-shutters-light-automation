/*
 for Xboard Rleay v3.0 [DFRobot]
 
 we could send simple http commands :
 
 /
 /set
 /get
 /help		-- list all availables commands
 
 --
 @TODO:
 
 + modular UDP ntp time client via a library
 + make lib for ambiant LED light
 + read temperature data
 - auto refresh sensor read at a defined rate
 - publish to pachube temperature and light data
 - implement REST help method
 - implement REST close-shutters method
 - find a beautiful case !
 
 --
 
 

 adapted from
 DFRobot X-board V2 Sample Code
 
 A simple web server with DHPC capbabilty.
 1)Get IP address from router automatically
 2)Show the value of the analog input pins
 
 created 28 Sep 2012
 by Ricky
 */

#include <SPI.h>
#include <Ethernet.h>

/**
 * personnals libs
 */
 // comment to disable debug mode
#define DEBUG
 
#include "debug.h"
#include "ntp_time.h"
#include "light_sensor.h"

#include <OneWire.h>
#include "temperature_sensor.h"


//#include "http_action.h"

EthernetServer server(80);
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xCD, 0xAE, 0x0F, 0xFE, 0xED };

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):

int ledPin = 13;                 // LED connected to digital pin 13





// pin for light sensor
#define LIGHT_SENSOR_ANALOG_INPUT 0

// pin for temperature sensor
#define TEMPERATURE_SENSOR_DIGITAL_PIN 4
TemperatureSensor temp(TEMPERATURE_SENSOR_DIGITAL_PIN);

// last NTP time read
String time = "n/a";



/**
 * possible actions on REST API
 */
enum actions {
  A_UNDEF,
  A_READ,
  A_SET,
  A_HELP,
  A_REFRESH_TIME,
  A_CLOSE_SHUTTERS
};


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    _LOG("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print your local IP address:
  _LOG("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    _LOG((Ethernet.localIP()[thisByte], DEC));
    //_LOG(printf("%d", Ethernet.localIP()[thisByte], DEC));
    _LOG("."); 
  }


  // start the Ethernet connection and the server:

  server.begin();
  //_LOG("server is at ");
  //_LOG_LN(Ethernet.localIP());

  int ledPin = 13;                 // LED connected to digital pin 13
  digitalWrite(ledPin, HIGH);   // sets the LED on
  Serial.println("Setup Done !");

	// ntp initialisation	
	setup_ntp_time();
	
	// light_sensor setup
	setup_light_sensor(LIGHT_SENSOR_ANALOG_INPUT, 255);

	_LOG_LN("Setup Done !");
}


void send_pin_data(EthernetClient client) {

  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connnection: close");
  client.println();
  client.print("{");
  // add a meta refresh tag, so the browser pulls again every 5 seconds:
  // output the value of each analog input pin
  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
    int sensorReading = analogRead(analogChannel);
    client.print("\"analog_input_");
    client.print(analogChannel);
    client.print("\":");
    client.print(sensorReading);
    client.print(",");      

  }

  // output the value of each digital input pin
  for (int digitalChannel = 2; digitalChannel < 13; digitalChannel++) {
    int sensorReading = digitalRead(digitalChannel);
    if(digitalChannel!=7&&digitalChannel!=8&&digitalChannel!=10)
    {
      client.print("\"digital_input_");
      client.print(digitalChannel);
      client.print("\":");
      client.print(sensorReading);
      client.print(","); 
    }
    else if(digitalChannel == 10) {
      continue;
    }
    else
    {
      client.print("\"relay_");
      client.print(digitalChannel);
      client.print("\":");
      client.print(sensorReading);
      client.print(","); 

    }


  }

	client.print("\"time\":\"");
	client.print(time);
	client.print("\",");

	client.print("\"nop\":0}");
  //client.flush();
}


//------------------------------------------------------------------------------

/**
 * main loop program
 */
void loop() {

	// read light sensor level (0-100) and wait for 2 sec
	_LOG("light level : ");
	_LOG_LN(get_light_level());	
	
	// read temperature from sensor
	_LOG("temperature : ");
	_LOG_LN(temp.get());	
	
	delay(2000);
	
	

	// listen for incoming clients
	EthernetClient client = server.available();
	if (client) {

 

    Serial.println("new client");
    String request = ""; // request line
   actions action = A_UNDEF;

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
	while (client.connected()) {
		if (client.available()) {
			char c = client.read();
			request += c;
			Serial.write(c);

			// if you've gotten to the end of the line (received a newline
			// character) and the line is blank, the http request has ended,
			// so you can send a reply
			if (c == '\n' && currentLineIsBlank) {

				switch(action) {

					case A_SET:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-Type: application/json");
						client.println("Connnection: close");
						client.println();
						client.print("{\"error\":\"tell me more\"}");
						break;
					case A_READ:
						send_pin_data(client);
						break;

					case A_REFRESH_TIME:
						time = String(getNtpTime());
						break;

					default:
						break;          
				}
			break; // break while loop
		}
        
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          if(request.startsWith("GET /")) {
            // we've got all the request line...



            String s_action = request.substring(4, request.lastIndexOf(" "));
            s_action.toLowerCase();
           
            if(s_action == "/set") {
             	action = A_SET;
            }
            else if(s_action == "/help") {
            	action = A_HELP;
            }
            else if(s_action == "/close-shutters"){
            	action == A_CLOSE_SHUTTERS;
            }
            else if(s_action == "/get"){
              	action = A_READ;
            }
            else if(s_action == "/update-time"){
            	action = A_REFRESH_TIME;
            }
            else {
            	action = A_HELP;
            }

            Serial.println("request is "+s_action);

          }
          request = ""; //
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(2);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


