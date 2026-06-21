// FIFO1.c
// Runs on any microcontroller
// Provide functions that implement the Software FiFo Buffer
// Last Modified: April 7, 2026
// Kevin Li, Anna Guan
#include <stdint.h>


// Declare state variables for FiFo
//        size, buffer, put and get indexes
static char FiFo[16];
static uint32_t put;
static uint32_t get;
// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void Fifo1_Init(){
//Complete this
 put = 0;
 get = 0;

}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
uint32_t Fifo1_Put(char data){
  //Complete this routine
  uint32_t nextspot = (put+1) % 16;
  if(nextspot == get){
    return 0;
  }
  FiFo[put] = data;
  put = nextspot;
  return 1; // replace this line with your solution

}

// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
char Fifo1_Get(void){
  //Complete this routine
  if(put == get){
    return 0;
  }
  char data = FiFo[get];
  get = (get+1) % 16; //to make the code circular
  return data;

}