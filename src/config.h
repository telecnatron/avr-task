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
#define LED_DEFS
// These are for atmega pro mini board
//! Output port to which LED is connected.
#define LED_W    PORTB
//! Input port to which LED is connected.
#define LED_R    PINB
//! Pin number of the port to which LED is connected.
#define LED_PIN  PIN5
//! DDR of the port to which LED is connected.
#define LED_CTL  DDRB


// tasks
#define TASK_WRITER     0
#define TASK_BLINKY_LED 1
#define TASK_DOTTY      2
#define TASK_BLINKING_CONTROL 3
#define TASK_WINKY      4
// Don't forget to update TASK_NUM_TASK whenever tasks are added or subtracted
#define TASK_NUM_TASKS 5
