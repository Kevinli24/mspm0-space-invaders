// Lab9Main.c
// Runs on MSPM0G3507
// Lab 9 ECE319K
// Your name
// Last Modified: January 12, 2026

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "FIFO1.h"
#include "../inc/DAC5.h"
#include "../inc/Arabic.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
//#include "images/images.h"
#include "GameController.h"
//NEED TO EXTERN LIKE THIS FOR ALL OF THE SPRITES
extern const uint16_t submarine[];
extern const uint16_t bullet[];
extern const uint16_t lazer[];
extern const uint16_t bomb[];

extern sprite_t Player;
extern sprite_t Lasers[];
extern sprite_t Bullets[];
extern sprite_t Bombs[];
extern phase_t GameState;
extern uint32_t Score;
extern uint32_t HighScore;
extern uint32_t GameTime;
extern uint32_t UltTimer;
extern uint32_t UltCooldown;

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}



Arabic_t ArabicAlphabet[]={
alif,ayh,baa,daad,daal,dhaa,dhaal,faa,ghayh,haa,ha,jeem,kaaf,khaa,laam,meem,noon,qaaf,raa,saad,seen,sheen,ta,thaa,twe,waaw,yaa,zaa,space,dot,null
};
Arabic_t Hello[]={alif,baa,ha,raa,meem,null}; // hello
Arabic_t WeAreHonoredByYourPresence[]={alif,noon,waaw,ta,faa,raa,sheen,null}; // we are honored by your presence
int main0(void){ // main 0, demonstrate Arabic output
  Clock_Init80MHz(0);
  LaunchPad_Init();
  ST7735_InitR(INITR_BLACKTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(ST7735_WHITE);
  Arabic_SetCursor(0,15);
  Arabic_OutString(Hello);
  Arabic_SetCursor(0,31);
  Arabic_OutString(WeAreHonoredByYourPresence);
  Arabic_SetCursor(0,63);
  Arabic_OutString(ArabicAlphabet);
  while(1){
  }
}


uint32_t collided = 0;
uint32_t ADCflag = 0;

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    uint32_t data, postC;
    
    data = ADCin();
    postC = Convert(data);
    Fifo1_Put(postC);
/*
    // 2) read input switches
    //CHANGE THE MASK AS NEEDED FOR WHEN YOU ADD SWITCHES
    uint32_t switchRead;
    if(switchRead & 0x01) Game_Shoot();
    if(switchRead & 0x02) Game_Ult();
    // 3) move sprites
    Game_Update();
    // 4) start sounds
    if(collided == 1){
      Sound_Killed();
      collided = 0;
    }
*/
    // 5) set semaphore
    ADCflag = 1;
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French, Chinese} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};
// use main1 to observe special characters

int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_BLACKTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(0x0000);            // set screen to black
  for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}
/*
// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  //ST7735_DrawBitmap(100, 159, lazer, 16,13); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, submarine, 16,16); 


  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=1000;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  printf("Is this horizontal?");
  //ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  
  while(1){
  }
}
*/
//use main3 to test switches and LEDs
int main3(void){ // main3
  uint32_t sw;  
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    sw = Switch_In(); 
    // Switch RIGHT (PA24) -> D1 (PA15, RED)
    if(sw & 0x01) LED_On(1<<15); else LED_Off(1<<15);
    
    // Switch LEFT (PA25) -> D2 (PA16, YELLOW)
    if(sw & 0x02) LED_On(1<<16); else LED_Off(1<<16);
    
    // Switch UP (PA26) -> D3 (PA17, GREEN)
    if(sw & 0x04) LED_On(1<<17); else LED_Off(1<<17);
    
    // Switch DOWN (PA27) -> no PCB LED so used launchpad green LED
    if(sw & 0x08) GPIOB->DOUTSET31_0 = GREEN; else GPIOB->DOUTCLR31_0 = GREEN;
  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_BLACKTAB);
  ST7735_FillScreen(ST7735_BLACK);
  ADCinit();     //PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
  Game_Init();
    // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_IntArm(80000000/30,2);
  // initialize all data structures
  //ST7735_DrawBitmap(Player.x, Player.y, Player.image, Player.w, Player.h);
  __enable_irq();
    while(1) {
      uint32_t sw = Switch_In();
      static uint32_t lastSW3 = 0;
      if((sw & 0x08) && !lastSW3) Language ^= 1;
      lastSW3 = sw & 0x08;

      //if(Fifo1_Peek() < 5) sw |= 0x01;   // Slide pot top = Start
      //if(Fifo1_Peek() > 105) sw |= 0x02; // Slide pot bottom = Shoot

      //start
      static uint32_t lastSW1 = 0;
      if((sw & 0x01) && !lastSW1){
      if(GameState == GAMEOVER){
          GameState = MENU;
          ST7735_FillScreen(ST7735_BLACK);
      } else if(GameState == MENU){
          Game_Init();
          GameState = PLAYING;
          ST7735_FillScreen(ST7735_BLACK);
      }
}
lastSW1 = sw & 0x01;

      while(ADCflag == 0){}
      ADCflag = 0;

      switch(GameState){
        case MENU:
            LED_Off(1<<15);
            LED_Off(1<<17);
            LED_On(1<<16); // LED YELLOW
            ST7735_SetRotation(1);
            ST7735_SetCursor(3, 4);
            printf(Language ? "SUBMARINO" : "SUB SURVIVAL");
            ST7735_SetCursor(2, 7);
            printf(Language ? "Mejor: %u     " : "High Score: %u     ", HighScore);
            ST7735_SetCursor(1, 11);
            printf(Language ? "Btn 4 para jugar   " : "Btn 4 to Start    ");
            ST7735_SetRotation(0);
            break;

        case PLAYING:
            LED_Off(1<<15);
            LED_Off(1<<16);
            LED_On(1<<17); // LED GREEN
            
            if(sw & 0x02) Game_Shoot(); 
            if(sw & 0x04) Game_Ult();

            
            
            Game_Update();     // Spawning, Moving, Timers
            CheckCollisions(); // Hit detection
            if(ShootFlag){ Sound_Shoot(); ShootFlag = 0; }
            if(DeathFlag){ Sound_Killed(); DeathFlag = 0; }

            
            MoveSubmarine();   // Pull from FIFO -> Player.x
            // Draw Player
            // Draw Player
            ST7735_FillRect(Player.oldX - 2, Player.y - (Player.h - 1), Player.w + 4, Player.h, ST7735_BLACK);
            ST7735_DrawBitmap(Player.x, Player.y, Player.image, Player.w, Player.h);
            Player.oldX = Player.x;
            // Draw Bombs (Bunker0)
            for(int i=0; i<MAX_BOMBS; i++){
                if(Bombs[i].status == alive){
                    ST7735_FillRect(Bombs[i].x, Bombs[i].oldY, Bombs[i].w, Bombs[i].h, ST7735_BLACK);
                    ST7735_DrawBitmap(Bombs[i].x, Bombs[i].y, Bombs[i].image, Bombs[i].w, Bombs[i].h);
                } else if(Bombs[i].oldY != 0){
                    ST7735_FillRect(Bombs[i].x, Bombs[i].oldY, Bombs[i].w, Bombs[i].h, ST7735_BLACK);
                    Bombs[i].oldY = 0;
                }
            }

            // Draw Obstacle Lasers (5x48)
            for(int i=0; i<MAX_LASERS; i++){
                if(Lasers[i].status == alive){
                    ST7735_FillRect(Lasers[i].x, Lasers[i].oldY, Lasers[i].w, Lasers[i].h, ST7735_BLACK);
                    ST7735_DrawBitmap(Lasers[i].x, Lasers[i].y, Lasers[i].image, Lasers[i].w, Lasers[i].h);
                } else if(Lasers[i].oldY != 0){
                    ST7735_FillRect(Lasers[i].x, Lasers[i].oldY, Lasers[i].w, Lasers[i].h, ST7735_BLACK);
                    Lasers[i].oldY = 0;
                }
            }

            // Draw Bullets (2x2)
            for(int i=0; i<MAX_BULLETS; i++){
                if(Bullets[i].status == alive){
                    ST7735_FillRect(Bullets[i].x, Bullets[i].oldY- (Bullets[i].h - 1), Bullets[i].w, Bullets[i].h, ST7735_BLACK);
                    ST7735_DrawBitmap(Bullets[i].x, Bullets[i].y, Bullets[i].image, Bullets[i].w, Bullets[i].h);
                    //Bullets[i].oldY = Bullets[i].y;
                } else if(Bullets[i].oldY != 0){
                    ST7735_FillRect(Bullets[i].x, Bullets[i].oldY, Bullets[i].w, Bullets[i].h, ST7735_BLACK);
                    Bullets[i].oldY = 0;
                }
            }

            // Draw HUD
            ST7735_SetRotation(1);
            ST7735_SetCursor(0, 0);
            if (UltTimer > 0) {
              printf(Language ? "Pts:%u ULT:%2us!" : "Hits:%u ULT:%2us!", Score, UltTimer/30);
            } else if (UltCooldown > 0) {
              printf(Language ? "Pts:%u CD:%2us  " : "Hits:%u CD:%2us ", Score, UltCooldown/30);
            } else {
              printf(Language ? "Pts:%u ULT:LISTO" : "Hits:%u ULT:RDY ", Score);
            }
            ST7735_SetRotation(0);
            break;

        case GAMEOVER:
            LED_Off(1<<15);
            LED_Off(1<<17);
            LED_On(1<<15); // (RED LED)
            if(Score > HighScore) HighScore = Score;
            ST7735_SetRotation(1);
            ST7735_SetCursor(5, 5);
            printf(Language ? "CHOCASTE!" : "CRASHED!");
            ST7735_SetCursor(2, 8);
            printf(Language ? "Puntos: %u        " : "Final Hits: %u    ", Score);
            ST7735_SetCursor(2, 12);
            printf(Language ? "Btn 4 para men\xA3" : "Btn 4 for menu  ");

            ST7735_SetCursor(0, 0);
            if (UltTimer > 0) {
              printf(Language ? "Pts:%u ULT:%2us!" : "Hits:%u ULT:%2us!", Score, UltTimer/30);
            } else if (UltCooldown > 0) {
              printf(Language ? "Pts:%u CD:%2us  " : "Hits:%u CD:%2us ", Score, UltCooldown/30);
            } else {
              printf(Language ? "Pts:%u ULT:LISTO" : "Hits:%u ULT:RDY ", Score);
            }
            ST7735_SetRotation(0);
            break;
    }
    /*
    MoveSubmarine();
    if(Player.x != Player.oldX){ //moved
      ST7735_FillRect(Player.oldX, Player.y - (Player.h - 1), Player.w, Player.h, ST7735_BLACK); //erase
      ST7735_DrawBitmap(Player.x, Player.y, Player.image, Player.w, Player.h); //16x16 can change
          //drew new
      Player.oldX = Player.x;
    } //ST7735_Black to be changed to diff color later LOLOLOL

    // 3. Game Logic & Drawing
    //Game_Update();
    //Game_Draw();
    
    //Clock_Delay1ms(33); // 30 FPS
    */
}
  
}
