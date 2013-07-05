/**
 * to use with sensor DFR0026 from DF Root
 */


#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H


unsigned char light_sensor_pin = 255;
unsigned long light_sensor_current_level = 0;
unsigned long light_sensor_range = 100;


/**
 * store light sensor pin and range for output value
 */
void
setup_light_sensor(unsigned char pin, unsigned long range = 100) {
	light_sensor_pin = pin;
	light_sensor_range = range;
}

/**
 * return light level 0-range (default=100)
 */
unsigned char
get_light_level(){
	if(light_sensor_pin == 255) {
		_LOG_LN("Please setup light sensor pin via setup_light_sensor() function");
		return 0;
	}
	light_sensor_current_level = analogRead(light_sensor_pin);
	light_sensor_current_level = 1024 - light_sensor_current_level;
	// no scaled value if range == analog range
	if(light_sensor_range == 1024) return light_sensor_current_level;

	return light_sensor_current_level * light_sensor_range / 1024;
}

#endif
