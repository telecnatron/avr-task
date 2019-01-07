// ------------------------------------------------------------------------------
// Copyright 2018 Stephen Stebbing. telecnatron.com
//
//    Licensed under the Telecnatron License, Version 1.0 (the “License”);
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        https://telecnatron.com/software/licenses/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an “AS IS” BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// ------------------------------------------------------------------------------

#include <avr/interrupt.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "./lib/log.h"
#include "./lib/stdout.h"
#include "./lib/sysclk.h"
#include "./lib/uart/uart.h"
#include "./lib/util/io.h"

// XXX debug
#include <util/delay.h>
#include "./lib/devices/LED.h"
#include "./lib/task.h"

// uart buffer size 
#define UART_RXBUF_SIZE 32

// forward declerations
void task_winky_off(void *);

//! enable uart, set up stdout
void _uart_init()
{
    // buffer for uart rx buffer
    static char rxbuf[UART_RXBUF_SIZE];
    // init uart
    uart_init(rxbuf,UART_RXBUF_SIZE);
    STDOUT_INIT();
}


void task_blinky_led(void *data)
{
    LED_TOGGLE();
    task_set_tick_timer(500);
}

void task_dotty(void *data)
{
    PUTC('.');
    task_set_tick_timer(400);
}

void task_blinking_control(void *data)
{
    static uint8_t c;
    if(c == 3 ){
	// make blinky unready
	task_num_ready(TASK_BLINKY_LED, 0);
	// make blinky ready
	task_num_ready(TASK_WINKY,1);
	LOG_INFO_FP("stopped blinking, started winking", NULL);
    } else if (c == 5) {
	// make blinky ready again
	task_num_ready(TASK_BLINKY_LED, 1);
	// make blinky unready
	task_num_ready(TASK_WINKY,0);
	LOG_INFO_FP("started blinking, stopped winking", NULL);
    } else if (c==8){
	c=0;
    }
    c++;
    task_set_seconds_timer(1);

}

void task_writer(void *data)
{
    LOG_INFO_FP("message %u", (*(uint8_t *)data)++ );
    // periodically stop blinky for a little while.
    task_set_seconds_timer(2);
}


void task_winky_on(void *data)
{
    LED_ON();
    task_set_tick_timer(10);
    // change this task's callback to task_winky_off()
    task_set_callback(task_winky_off);
}

void task_winky_off(void *data)
{
    LED_OFF();
    task_set_tick_timer(250);
    // change this task's callback to task_winky_on()
    task_set_callback(task_winky_on);
}

int main()
{

    // onboard LED
    LED_INIT();
    LED_ON();

    // ticker, uses timer2
    sysclk_init();

    // init uart, stdout
    _uart_init();

    // logger
    log_set_level(LOG_LEVEL_INFO);

    // tasks
    // initialise writer task and make it ready to run
    uint8_t writer_count =0;
    task_init(TASK_WRITER, task_writer, (void *)&writer_count, 1);
    task_init(TASK_BLINKY_LED, task_blinky_led, NULL, 1);
    task_init(TASK_DOTTY, task_dotty, NULL, 1);
    task_init(TASK_BLINKING_CONTROL, task_blinking_control, NULL, 1);
    task_init(TASK_WINKY, task_winky_on, NULL, 1);

    // enable global interrupts
    sei();
    LOG_INFO_FP(" --- INITIALISED --- ", NULL);


    for(;;){
	if(sysclk_has_ticked()){
	    // this block called about every 1ms
	    task_tick();
	    if( sysclk_have_seconds_ticked()){
		// this block called every second
		task_seconds_tick();
	    }
	    task_run();
	}
	// sleep here
    }
    return 0;
}




