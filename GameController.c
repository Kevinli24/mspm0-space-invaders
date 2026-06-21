#include "GameController.h"
#include "../inc/ST7735.h"
#include "../inc/ADC1.h"
#include "Switch.h"
#include "FIFO1.h"



sprite_t Player;
sprite_t Lasers[MAX_LASERS];
sprite_t Bullets[MAX_BULLETS];
sprite_t Bombs[MAX_BOMBS];

sprite_t Enemy;
uint32_t Score;
uint32_t UltTimer;    // Countdown for how long Ult lasts
uint32_t UltCooldown; // Countdown until next Ult is ready
uint32_t HighScore;  // Best hits ever
uint32_t GameTime;   // Current survival time (ticks)
uint32_t Language = 0;
uint32_t ShootFlag = 0;
uint32_t DeathFlag = 0;

uint32_t BombSpawnTimer = 0;
uint32_t LaserSpawnTimer = 0;

void Game_Init(void){
    GameState = MENU;
    Score = 0;
    GameTime = 0;

    Player.x = 60;      
    Player.y = 30;      //fixed
    Player.oldX = Player.x + 1;
    Player.vx = 0;
    Player.vy = 0;   
    //SIZE CHANGE HERE IN GAMECONTROLLER
    Player.w = 8;      // Your 16x16 size
    Player.h = 18;
    Player.image = submarine; // The name of your converted BMP array
    Player.status = alive;
    
    for(int i = 0; i < MAX_BULLETS; i++){
        Bullets[i].status = dead;
        Bullets[i].image = bullet;
        Bullets[i].w = 1;
        Bullets[i].h = 8;
    } 

    for(int i = 0; i < MAX_LASERS; i++) {
        Lasers[i].status = dead;  
        Lasers[i].image = lazer;
        Lasers[i].w = 48;
        Lasers[i].h = 10;  
    }
    
    for(int i = 0; i < MAX_BOMBS; i++){
        Bombs[i].status = dead;
        Bombs[i].image = bomb;
        Bombs[i].w = 10;
        Bombs[i].h = 10;
    }

    UltTimer = 0;
    UltCooldown = 0;
    
}
void MoveSubmarine(void) {
    
    char data = Fifo1_Get();
    Player.x = (uint32_t)data;
    
   
    // 1. Get data from your SlidePot FIFO
    
}

// Logic for firing a standard shot
void Game_Shoot(void){
    for(int i=0; i<MAX_BULLETS; i++){
        if(Bullets[i].status == dead){
            ShootFlag = 1;
            Bullets[i].status = alive;
            Bullets[i].x = Player.x + (Player.w/2) - 1; //centers it vert
            Bullets[i].y = Player.y + Player.h; //puts it in the front horiz
            Bullets[i].vx = 4; //speed
            break; // Only fire one per button press
        }
    }
}

// Logic for an Ultimate Ability (e.g., clear screen)
void Game_Ult(void){
    if(UltCooldown == 0 && UltTimer == 0){
        UltTimer = 300; // 10 seconds of active "God Mode"
        
        // Visual Clear
        ST7735_FillScreen(ST7735_BLACK); 
        for(uint16_t i = 0; i < MAX_BOMBS; i++) Bombs[i].status = dead;
        for(uint16_t i = 0; i < MAX_LASERS; i++) Lasers[i].status = dead;
        
        // Redraw Player immediately
        ST7735_DrawBitmap(Player.x, Player.y, Player.image, Player.w, Player.h);
    }
}
uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}
void Game_Spawn(void) {
    if(UltTimer > 0) return;

    BombSpawnTimer++;
    if(BombSpawnTimer > 60) {
        for(int i=0; i<MAX_BOMBS; i++) {
            if(Bombs[i].status == dead) {
                Bombs[i].status = alive;
                Bombs[i].y = 159;         // Start at far right
                // Random vertical lane (0 to 128-16)
                Bombs[i].x = Random(112); 
                BombSpawnTimer = 0;
                break; // Only spawn one
            }
        }
    }

    // 2. Spawn Obstacle Lasers (Every ~5 seconds, or 150 frames)
    LaserSpawnTimer++;
    if(LaserSpawnTimer > 150) {
        for(int i=0; i<MAX_LASERS; i++) {
            if(Lasers[i].status == dead) {
                Lasers[i].status = alive;
                Lasers[i].y = 159; // Start at far right
                // Random vertical position (0 to 128-48)
                Lasers[i].x = Random(80); 
                LaserSpawnTimer = 0;
                break;
            }
        }
    }
}

void Game_Update(void) {
    // Handle bullets (Y increases to move right)
    if(GameState != PLAYING) return;
    Game_Spawn();

    for(int i=0; i<MAX_BULLETS; i++) {
        if(Bullets[i].status == alive) {

            Bullets[i].oldY = Bullets[i].y;
            Bullets[i].y += 4; // Laser speed
            if(Bullets[i].y > 159) Bullets[i].status = dead; //reached other side
            
        }
    }

    // Handle Bombs (Y decreases to move left)
    for(int i=0; i<MAX_BOMBS; i++) {
        if(Bombs[i].status == alive) {
            Bombs[i].oldY = Bombs[i].y;
            Bombs[i].y -= 2; // Obstacle speed
            if(Bombs[i].y < 0) Bombs[i].status = dead;
        }
    }

    for(int i = 0; i< MAX_LASERS; i++){
        if(Lasers[i].status == alive){
            Lasers[i].oldY = Lasers[i].y;
            Lasers[i].y -= 1;
            if(Lasers[i].y < 0) Lasers[i].status = dead;
        }
    }

    // Timers
    if(UltTimer > 0){
        UltTimer--; 
        if(UltTimer == 0){
            // THE MOMENT the Ult ends, we set the cooldown
            UltCooldown = 600; // 20 seconds
        }
    } else if(UltCooldown > 0){
        UltCooldown--; // Only counts down if UltTimer is already 0
    }
}

phase_t GameState = MENU;
uint32_t Score = 0;
uint32_t HighScore = 0;
uint32_t GameTime = 0;

void Game_StartTrigger(void) {
    if (GameState == MENU || GameState == GAMEOVER) {
        // Reset game variables
        Score = 0;
        GameTime = 0;
        Player.status = alive;
        // Clear lasers and bombs
        for(int i=0; i<MAX_LASERS; i++) Lasers[i].status = dead;
        for(int i=0; i<MAX_BOMBS; i++) Bombs[i].status = dead;
        for(int i = 0; i < MAX_BULLETS; i++) Bullets[i].status = dead;
        
        GameState = PLAYING; // Now the button does "nothing" while playing
    }
}

uint8_t DetectOverlap(int32_t x1, int32_t y1, uint32_t w1, uint32_t h1, 
                     int32_t x2, int32_t y2, uint32_t w2, uint32_t h2) {
    return (x1 < x2 + w2) && (x1 + w1 > x2) && 
           (y1 < y2 + h2) && (y1 + h1 > y2);
}

void CheckCollisions(void){
    if (GameState != PLAYING) return;

    // 1. Check Player vs. Bombs
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (Bombs[i].status == alive) {
            if (DetectOverlap(Player.x, Player.y, Player.w, Player.h, 
                              Bombs[i].x, Bombs[i].y, Bombs[i].w, Bombs[i].h)) {
                if (UltTimer == 0) {
                GameState = GAMEOVER; // Death!
                DeathFlag = 1;
                }
            }
        }
    }

    // 2. Check Player vs. Obstacle Lasers (Invincible to bullets)
    for (int i = 0; i < MAX_LASERS; i++) {
        if (Lasers[i].status == alive) {
            if (DetectOverlap(Player.x, Player.y, Player.w, Player.h, 
                              Lasers[i].x, Lasers[i].y, Lasers[i].w, Lasers[i].h)) {
                if (UltTimer == 0) {
                GameState = GAMEOVER; // Death!
                DeathFlag = 1;
                }
            }
        }
    }

    // 3. Check Bullets vs. Bombs ONLY
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (Bullets[i].status == alive) {
            for (int j = 0; j < MAX_BOMBS; j++) {
                if (Bombs[j].status == alive) {
                    if (DetectOverlap(Bullets[i].x, Bullets[i].y, Bullets[i].w, Bullets[i].h, 
                                      Bombs[j].x, Bombs[j].y, Bombs[j].w, Bombs[j].h)) {
                        ST7735_FillRect(Bullets[i].x, Bullets[i].y - (Bullets[i].h - 1), 
                                        Bullets[i].w, Bullets[i].h, ST7735_BLACK);
                        ST7735_FillRect(Bombs[j].x, Bombs[j].y - (Bombs[j].h - 2), 
                                        Bombs[j].w, Bombs[j].h, ST7735_BLACK);
                        Bullets[i].status = dead;
                        Bombs[j].status = dead;
                        Score++; // Point for shooting a bomb!
                    }
                }
            }
        }
    }
}
