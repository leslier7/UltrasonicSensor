/*
Interupt driven ultrasoinc sensor code by Robbie Leslie.
Based on RPM sensor code by John Nestor.
ECE414 - Fall 2023

*/

#include "stdio.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "timer.h"
#include "hardware/timer.h"

//Change these to the pins you want to use
//Pico pin 27
#define TRIG_PIN 21

//Pico pin 26
#define ECHO_PIN 20


#define GPIO_IRQ_EDGE_FALL 0x4u
#define GPIO_IRQ_EDGE_RISE 0x8u

#define NO_CAPTURE_READY 0xffffffff

#define MIN_US_TIMEOUT_US 100000

//Initialize the ultrasonic sensor, including the GPIO pins and the interrupt on the rising edge of the echo pin.
void us_init();

//Returns the period of the ultrasonic pulse in microseconds
uint32_t us_getperiod();

//Converts the time of the period to distance in cm. Returns NO_CAPTURE_READY if the period is not ready.
uint32_t time_to_cm(uint32_t time);

//Returns the distance in cm. Returns NO_CAPTURE_READY if the period is not ready.
uint32_t us_getdistance();

//Sends out a pulse on TRIG_PIN
void us_sendpulse();

//Setting up the callback for the rising edge of the echo pin. This is the start of the pulse.
void us_callback_rise(uint gpio, uint32_t event_mask);

//Setting up the callback for the falling edge of the echo pin. This is the end of the pulse.
void us_callback_fall(uint gpio, uint32_t event_mask);

//Tick function for the ultrasonic state machine. Minimum period for the state machine is 20ms.
void us_tick(uint32_t *distance);

//Callback for the alarm interupt. Turns off the pico pin.
alarm_callback_t pulse_callback(alarm_id_t id, void *user_data);