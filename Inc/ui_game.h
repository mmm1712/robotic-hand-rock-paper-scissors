#ifndef UI_GAME_H
#define UI_GAME_H

#include <stdint.h>

#define ROCK       0
#define PAPER      1
#define SCISSORS   2
#define NO_GESTURE 255

typedef enum {
    GAME_HOME,
    GAME_COUNTDOWN,
    GAME_WAIT_HAND,
    GAME_TRY_AGAIN
} GameState;
void UI_GameInit(void);
void UI_GameUpdate(void);

/* these are DEFINED in main.c */
extern volatile uint8_t externalGestureReady;
extern volatile uint8_t externalGesture;


#endif
