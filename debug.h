/**
 * this lib allow to DEBUG some strings to serial line
 *
 * use DEBUG : simply define DEBUG constant
 */

#ifndef DEBUG_H
#define DEBUG_H

// _LOG without end line
#ifdef DEBUG
	#define _LOG(x) Serial.print(x)
	#define _LOG_LN(x) Serial.println(x)
#else
	#define _LOG(x)
	#define _LOG_LN(x)
#endif

#endif
