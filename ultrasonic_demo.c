#include "ultrasonic.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "timer.h"

static uint32_t us_distance;
static uint32_t timer1, timer2;

int main(){
    const uint32_t us_period = 30; //Period for ultrasonic sensor needs to be at least 20ms or else it may have an error at the rated 2m range
    stdio_init_all();
    us_init();
    us_distance = 0;
    timer1 = timer_read();

    while(1){
        timer2 = timer_read();
        if(timer_elapsed_ms(timer1, timer2) >= us_period){
            us_tick(&us_distance);
            printf("Distance: %d cm\n", us_distance);
            timer1 = timer2;
        }
        
        
    }

}