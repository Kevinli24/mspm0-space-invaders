/*
 * Switch.c
 *
 *  Created on: January 12, 2026
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    IOMUX->SECCFG.PINCM[PA26INDEX] = 0x00040081; // GPIO input
    IOMUX->SECCFG.PINCM[PA25INDEX] = 0x00040081; // GPIO input
    IOMUX->SECCFG.PINCM[PA24INDEX] = 0x00040081; // GPIO input
    IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00040081; // GPIO input
}

// return current state of switches
uint32_t Switch_In(void){
// regular Lab5
  uint32_t key_input = GPIOA->DIN31_0 & 0x0F000000;
  key_input = key_input >> 24;
  return key_input; // replace this line
}
