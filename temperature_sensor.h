/**
 * to use with sensor DFR0024 from DF Root
 *
 * one wire lib from http://bildr.org/2011/07/ds18b20-arduino/
 * just put it into (home directory)/sketchbook/libraries
 */

#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

//#include <OneWire.h>

#define	TEMP_ERROR	255


class TemperatureSensor: public OneWire
{
    public:
      // constructor for the class that inherits OneWire methods 
      TemperatureSensor(uint8_t pin) : OneWire(pin) { }
      float get();                 // added get method for read temperature
};


/**
 * return temperature Degre Celsius
 */
float
TemperatureSensor::get(){

	byte data[12];
	byte addr[8];

	if ( !TemperatureSensor::search(addr)) {
		//no more sensors on chain, reset search
		TemperatureSensor::reset_search();
		return TEMP_ERROR;
	}

	if ( OneWire::crc8( addr, 7) != addr[7]) {
		_LOG_LN("temperature_sensor : CRC is not valid!");
		return TEMP_ERROR;
	}

	if ( addr[0] != 0x10 && addr[0] != 0x28) {
		_LOG("temperature_sensor : Device is not recognized");
		return TEMP_ERROR;
	}

	TemperatureSensor::reset();
	TemperatureSensor::select(addr);
	TemperatureSensor::write(0x44,1); // start conversion, with parasite power on at the end

	byte present = TemperatureSensor::reset();
	TemperatureSensor::select(addr);    
	TemperatureSensor::write(0xBE); // Read Scratchpad


	for (int i = 0; i < 9; i++) { // we need 9 bytes
		data[i] = TemperatureSensor::read();
	}

	TemperatureSensor::reset_search();

	byte MSB = data[1];
	byte LSB = data[0];

	float tempRead = ((MSB << 8) | LSB); //using two's compliment
	float TemperatureSum = tempRead / 16;

	return TemperatureSum;
}

#endif





  
  
  

