/*
 * LED.c
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table

// initialize your LEDs
void LED_Init(void){
    IOMUX->SECCFG.PINCM[PA15INDEX] = (uint32_t)0x00000081; // D1
    IOMUX->SECCFG.PINCM[PA16INDEX] = (uint32_t)0x00000081; // D2
    IOMUX->SECCFG.PINCM[PA17INDEX] = (uint32_t)0x00000081; // D3

    GPIOA->DOE31_0 |= (1<<15)|(1<<16)|(1<<17);

    // Start off
    GPIOA->DOUTCLR31_0 = (1<<15)|(1<<16)|(1<<17);
}
// data specifies which LED to turn on
void LED_On(uint32_t data){
    GPIOA->DOUTSET31_0 = (data & ((1<<15)|(1<<16)|(1<<17)));
}

// data specifies which LED to turn off
void LED_Off(uint32_t data){
    GPIOA->DOUTCLR31_0 = (data & ((1<<15)|(1<<16)|(1<<17)));
}

// data specifies which LED to toggle
void LED_Toggle(uint32_t data){
    GPIOA->DOUTTGL31_0 = (data & ((1<<15)|(1<<16)|(1<<17)));
}

