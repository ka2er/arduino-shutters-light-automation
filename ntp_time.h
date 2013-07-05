#include <EthernetUdp.h>


/* NTP part */

unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
/*
Non-authoritative answer:
Name:	fr.pool.ntp.org
Address: 88.190.34.44
Name:	fr.pool.ntp.org
Address: 193.55.167.2
Name:	fr.pool.ntp.org
Address: 88.191.120.99
Name:	fr.pool.ntp.org
Address: 88.190.238.142
*/
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

/* end of NTP client defs*/


/**
 * create an NTP request and send it
 */ 
void _requestNtpTime(IPAddress& address) {
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE); 
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49; 
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:         
	Udp.beginPacket(address, 123); // NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket(); 
}


/**
 * listen for NTP response and parse it
 */
unsigned long _checkForNtpResponse() {
  
	// wait to see if a reply is available
	delay(1000);  
	if ( Udp.parsePacket() ) {  

		// We've received a packet, read the data from it
		Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

		//the timestamp starts at byte 40 of the received packet and is four bytes,
		// or two words, long. First, esxtract the two words:
		unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
		unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
		// combine the four bytes (two words) into a long integer
		// this is NTP time (seconds since Jan 1 1900):
		unsigned long secsSince1900 = highWord << 16 | lowWord;  
		//time = "Seconds since Jan 1 1900 = ";
		//time += secsSince1900;               
		
		// now convert NTP time into everyday time:
		// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
		const unsigned long seventyYears = 2208988800UL;     
		// subtract seventy years:
		
		return secsSince1900 - seventyYears;;  
		// print Unix time:
		//time += "- Unix time = " + epoch;                               

		/*

		// print the hour, minute and second:
		time += "- The UTC time is "+ (epoch  % 86400L) / 3600; // print the hour (86400 equals secs per day)
		time += ':';  
		if ( ((epoch % 3600) / 60) < 10 ) {
		  // In the first 10 minutes of each hour, we'll want a leading '0'
		  time += '0';
		}
		time += (epoch  % 3600) / 60; // print the minute (3600 equals secs per minute)
		time += ':'; 
		if ( (epoch % 60) < 10 ) {
		  // In the first 10 seconds of each minute, we'll want a leading '0'
		  time += '0';
		}
		time += epoch %60; // print the second
		
		Serial.println(time);
		*/
	}
	return 0;
}


//------------------------------------------------------------------------------


/**
 * setup NTP lib
 */
void setup_ntp_time() {
	// UDP listening
	Udp.begin(localPort);
}

/**
 * read time from NTP server
 *
 * @return timestamp
 */
unsigned long getNtpTime() {
	
	// send request packet
	_requestNtpTime(timeServer);
	
	// listen for response
	_LOG("NTP Time is : ");
	unsigned long ts = _checkForNtpResponse();
	if(ts)
		_LOG(ts);
	else
		_LOG("n/a");
		
	return ts;
}


