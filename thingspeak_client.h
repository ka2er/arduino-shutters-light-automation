/**
 * to use with http://api.thingspeak.com
  */

#ifndef THINGSPEAK_CLIENT_H
#define THINGSPEAK_CLIENT_H

#include <SPI.h>
#include <Ethernet.h>

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";

// Variable Setup
EthernetClient client;

long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

const int maxUpdateThingSpeakInterval = 16 * 1000; 


void
thingspeak_post(String api_key, String sensor1,String sensor2) {

	long now = millis();
	if((now - lastConnectionTime) < maxUpdateThingSpeakInterval) {
		_LOG_LN("POST to thingspeak is too early : DENIED !");
		return;
	}

	String data = "field1="+sensor1+"&field2="+sensor2;
	

	if (client.connect(thingSpeakAddress, 80)) {         
		client.print("POST /update HTTP/1.1\n");
		client.print("Host: api.thingspeak.com\n");
		client.print("Connection: close\n");
		client.print("X-THINGSPEAKAPIKEY: "+api_key+"\n");
		client.print("Content-Type: application/x-www-form-urlencoded\n");
		client.print("Content-Length: ");
		client.print(data.length());
		client.print("\n\n");

		client.print(data);

		lastConnectionTime = millis();

		if (client.connected()) {
			_LOG_LN("Connecting to ThingSpeak...");
			failedCounter = 0;
			client.flush();
			client.stop();
		} else {
			failedCounter++;
			_LOG_LN("Connection to ThingSpeak failed (attempt #"+String(failedCounter, DEC)+")");   
		}

	} else {
		failedCounter++;

		_LOG_LN("Connection to ThingSpeak Failed (attempt #"+String(failedCounter, DEC)+")");   

		lastConnectionTime = millis(); 
	}
}
#endif
