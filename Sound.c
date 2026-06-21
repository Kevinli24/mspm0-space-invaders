// Sound.c
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

const uint8_t *SoundPt;
uint32_t SoundCount;

void SysTick_IntArm(uint32_t period, uint32_t priority){
  SysTick->LOAD = period - 1;
  SysTick->VAL = 0;
  SysTick->CTRL = 0x07;
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5-bit DAC
void Sound_Init(void){
  SoundPt = 0;
  SoundCount = 0;
  DAC5_Init();
  SysTick_IntArm(80000000/11000, 2);
}
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
    // output one value to DAC if a sound is active
  if(SoundCount > 0){
    DAC5_Out(*SoundPt);
    SoundPt++;
    SoundCount--;
    } else {
        DAC5_Out(0);
    }
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){
// write this
  SoundPt = pt;
  SoundCount = count;
}
void Sound_Shoot(void){
    Sound_Start(shoot, 4080);
}

void Sound_Killed(void){
    Sound_Start(invaderkilled, 3377);
}
void Sound_Explosion(void){
// write this
  Sound_Start(explosion, 4080);
}

void Sound_Fastinvader1(void){
  Sound_Start(fastinvader1, 4080);
}
void Sound_Fastinvader2(void){

}
void Sound_Fastinvader3(void){

}
void Sound_Fastinvader4(void){

}
void Sound_Highpitch(void){

}

