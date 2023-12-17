#include "ultrasonic.h"

static volatile uint32_t capture1, capture2, capture_period;
static volatile bool capture_flag1, capture_flag2;

//setting up the default alarm pool
static alarm_pool_t* default_alarm_pool;

//Writing the state machine for sending the ultrasonic pulses

enum States {init, send_pulse, calculate_distance} US_State;

//Tick function for the ultrasoinc sensor. Give it a pointer to an integer to store the distance in.
void us_tick(uint32_t *distance){
    //Transitions
    switch(US_State){
        case init:
            US_State = send_pulse;
            break;
        case send_pulse:
            US_State = calculate_distance;
            break;
        case calculate_distance:
            US_State = send_pulse;
            break;
        default:
            US_State = init;
            break;
    }

    //Actions
    switch(US_State){
        case init:
            break;
        case send_pulse:
            //printf("Sending pulse\n");
            us_sendpulse();
            break;
        case calculate_distance:
            *distance = us_getdistance();
            break;
        default:
            break;
    }
}

//Setting up the callback for the rising edge of the echo pin. This is the start of the pulse.
void us_callback_rise(uint gpio, uint32_t event_mask){
    capture1 = time_us_32();
    //printf("Capture 1: %d us\n", capture1);
    //printf("rise callback reached\n");
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE, false, &us_callback_rise);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_FALL, true, &us_callback_fall);
    capture_flag1 = true;

}

//Setting up the callback for the falling edge of the echo pin. This is the end of the pulse.
void us_callback_fall(uint gpio, uint32_t event_mask){
    capture2 = time_us_32();
    //printf("Capture 2: %d us\n", capture2);
    //printf("fall callback reached\n");
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_FALL, false, &us_callback_fall);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE, true, &us_callback_rise);
    capture_flag2 = true;
}

//Initialize the ultrasonic sensor, including the GPIO pins and the interrupt on the rising edge of the echo pin.
void us_init(){
    default_alarm_pool = alarm_pool_get_default();
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE, true, &us_callback_rise);
    US_State = init;
    printf("Ultrasonic sensor initialized\n");
    
    capture1 = 0;
    capture2 = 0;
    capture_period = 0;
    capture_flag1 = false;
    capture_flag2 = false;
}

//Returns the period of the ultrasonic pulse in microseconds
uint32_t us_getperiod(){
    if(capture_flag1 && capture_flag2){

        //Tried to fix what happens if capture1 is bigger than capture2, but this didnt work.
        capture_period = capture2 - capture1;
        
        //Can use these print statements for debugging
        //printf("Capture 1: %d us\n", capture1);
        //printf("Capture 2: %d us\n", capture2);
        //printf("Capture period: %d us\n", capture_period);
        capture_flag1 = false;
        capture_flag2 = false;
        return capture_period;
    } else {
        return NO_CAPTURE_READY;
    }
}

static uint32_t distance;
//static uint32_t speed_of_sound = 34300; //cm/s

//Converts the time of the period to distance in cm. Returns NO_CAPTURE_READY if the period is not ready.
uint32_t time_to_cm(uint32_t time){
    if (time == NO_CAPTURE_READY){
        return NO_CAPTURE_READY;
    } else {
        distance = (time /58);
        return distance;
    }
}

//Returns the distance in cm. Returns NO_CAPTURE_READY if the period is not ready.
uint32_t us_getdistance(){
    uint32_t period = us_getperiod();
    if (period != NO_CAPTURE_READY){
        return time_to_cm(period);
    } else {
        return NO_CAPTURE_READY;
    }
    
}


static const uint32_t timer_period_us = 10;

//Sends out a pulse on TRIG_PIN
void us_sendpulse(){
    gpio_put(TRIG_PIN, 1);
    //Less efficient way of doing it. I left it here for reference.
    //sleep_us(10);
    //gpio_put(TRIG_PIN, 0);
    
    //Sets an alarm interupt to turn off the pico pin after 10 us
    add_alarm_in_us(timer_period_us, pulse_callback, NULL, true);
    //When compiling, this does give an error about the pulse_callback function being the wrong type, but the code still works.

    //This is the code for the timer. It is not working yet. If I have this uncommented, it will compile, but the code doesn't seem to enter the us_sendpulse function.
    //add_repeating_timer_us(-timer_period_us, repeating_timer_callback, NULL, &timer);
    //printf("Pulse sent\n");

}

//Callback for the alarm interupt. Turns off the pico pin.
alarm_callback_t pulse_callback(alarm_id_t id, void *user_data){
    gpio_put(TRIG_PIN, 0);
    //printf("Callback reached\n");
    return NULL;
}

