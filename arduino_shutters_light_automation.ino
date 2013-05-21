/*

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
EthernetServer server(80);
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xCD, 0xAE, 0x0F, 0xFE, 0xED };
 
// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
 
 
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
  ; // wait for serial port to connect. Needed for Leonardo only
  }
  
 
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
 
  // start the Ethernet connection and the server:
  
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}
 
 
void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
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
          for (int digitalChannel = 2; digitalChannel < 10; digitalChannel++) {
            int sensorReading = digitalRead(digitalChannel);
            if(digitalChannel!=7&&digitalChannel!=8)
            {
            client.print("\"digital_input_");
            client.print(digitalChannel);
            client.print("\":");
            client.print(sensorReading);
             client.print(","); 
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
           
           
           
          client.print("\"nop\":0}");
          //client.flush();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
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
