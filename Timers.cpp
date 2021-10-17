/*
  Timers.cpp - Base Class for Timerss.
  Created by Carlos V.
  Released into the public domain.
*/

#include "Timers.h"

#include "nrf.h"

void start_timer(void)
{		
  ////////// TIMER2
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;          // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                       // clear the task first to be usable for later
	NRF_TIMER2->PRESCALER = 0;                        // Set prescaler. Higher number gives slower timer. 
                                                     // Prescaler = 0 gives 16MHz timer. f = 16 MHz / 2^(n)
                                                     // f 1kHz ==> T 1ms ==> log2(16000) ==> 13.96
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit; // Set counter to 16 bit resolution
	NRF_TIMER2->CC[0] = 10000;                         // Set value for TIMER compare register 0
	NRF_TIMER2->CC[1] = 5;                             // Set value for TIMER compare register 1
		
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
	NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);
		
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER

  ////////// TIMER3
  // NRF_TIMER3->MODE = TIMER_MODE_MODE_Timer;          // Set the timer in Counter Mode
  // NRF_TIMER3->TASKS_CLEAR = 1;                       // clear the task first to be usable for later
	// NRF_TIMER3->PRESCALER = 0;                        // Set prescaler. Higher number gives slower timer. 
  //                                                    // Prescaler = 0 gives 16MHz timer. f = 16 MHz / 2^(n)
  //                                                    // f 1kHz ==> T 1ms ==> log2(16000) ==> 13.96
	// NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_16Bit; // Set counter to 16 bit resolution
	// NRF_TIMER3->CC[0] = 10000;                         // Set value for TIMER compare register 0
	// NRF_TIMER3->CC[1] = 5;                             // Set value for TIMER compare register 1
		
  // // Enable interrupt on Timer 3, both for CC[0] and CC[1] compare match events
	// NRF_TIMER3->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  // NVIC_EnableIRQ(TIMER2_IRQn);
		
  // NRF_TIMER3->TASKS_START = 1;               // Start TIMER
}
