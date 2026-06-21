#ifndef __GameController_h
#define __GameController_h

#include <stdint.h>

// Game Constants
#define MAX_LASERS 3
#define MAX_BOMBS 3
#define MAX_BULLETS 6

extern const uint16_t submarine[];
extern const uint16_t bullet[];
extern const uint16_t lazer[];
extern const uint16_t bomb[];
extern uint32_t Language; // 0 = English, 1 = Spanish
extern uint32_t ShootFlag;
extern uint32_t DeathFlag;

typedef enum {dead, alive} status_t;
typedef enum {MENU, PLAYING, GAMEOVER} phase_t;
extern phase_t GameState;

struct sprite {
    int32_t x;      // x-pos
    int32_t y;      // y-pos
    int32_t oldX;   // old x pos (for obstacles/bullets moving horizontally)
    int32_t oldY;   // old y pos (for player moving vertically)
    int32_t vx, vy;
    const uint16_t *image; 
    uint32_t w, h;
    status_t status; 
};
typedef struct sprite sprite_t;

extern sprite_t Player;
extern sprite_t Lasers[MAX_LASERS];
extern sprite_t Bombs[MAX_BOMBS];
extern sprite_t Bullets[MAX_BULLETS];
extern uint32_t Score;
extern uint32_t UltTimer;    // Countdown for how long Ult lasts
extern uint32_t UltCooldown; // Countdown until next Ult is ready
extern uint32_t HighScore;  // Best hits ever
extern uint32_t GameTime;   // Current survival time (ticks)

// Function Prototypes
void Game_Init(void);
void MoveSubmarine(void);
void Game_Update(void);
//void Game_Draw(void);
void Game_Shoot(void); // Triggered by a switch
void Game_Ult(void);   // Triggered by a switch
void CheckCollisions(void);
void Game_StartTrigger(void);

#endif