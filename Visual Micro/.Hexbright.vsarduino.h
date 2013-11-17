/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Hexbright, Platform=avr, Package=hexbright
*/

#define __AVR_ATmega168__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 8000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
void measure();
//
void tiltShow();
void xloop();

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\src\Arduino\Hexbright\Hexbright.ino"
#include "C:\src\Arduino\Hexbright\Acceleration.h"
#include "C:\src\Arduino\Hexbright\PowerButton.cpp"
#include "C:\src\Arduino\Hexbright\PowerButton.h"
#include "C:\src\Arduino\Hexbright\Timer.h"
#include "C:\src\Arduino\Hexbright\Vector.h"
#include "C:\src\Arduino\Hexbright\hexbright.h"
#include "C:\src\Arduino\Hexbright\hexlib.h"
