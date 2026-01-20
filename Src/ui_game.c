#include "ui_game.h"
#include "ui.h"
#include "touch.h"
#include "hand.h"
#include "tft.h"
#include "stm32f4xx_hal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern volatile uint8_t externalGesture;
extern volatile uint8_t externalGestureReady;

#define HOLD_MS            7000
#define COUNTDOWN_STEP_MS  1400


#define COUNTDOWN_SCALE    5
#define COUNTDOWN_Y        95
#define COUNTDOWN_MARGIN   8

/* result UI */
#define HOME_BTN_Y         210  /* DrawCenteredButton Y */
#define HOME_BTN_TOUCH_Y   200  /* touch threshold for "HOME area" */

/* ============================================================ */

static GameState state = GAME_HOME;

static uint8_t robotMove;
static uint8_t userMove;

static uint32_t timer;
static uint8_t  countdown;

/* gesture control */
static uint32_t gestureDeadline = 0;
static uint8_t  gestureAccepted = 0;

/* touch debounce */
static uint8_t touchWasDown = 0;

static const char* GestureName(uint8_t g)
{
    if (g == ROCK)     return "ROCK";
    if (g == PAPER)    return "PAPER";
    if (g == SCISSORS) return "SCISSORS";
    return "?";
}

static void ClearCountdownDigitArea(uint16_t x, uint16_t y, uint8_t scale)
{
    uint16_t w = 5 * scale;
    uint16_t h = 7 * scale;

    int16_t rx = (int16_t)x - COUNTDOWN_MARGIN;
    int16_t ry = (int16_t)y - COUNTDOWN_MARGIN;
    uint16_t rw = w + 2 * COUNTDOWN_MARGIN;
    uint16_t rh = h + 2 * COUNTDOWN_MARGIN;

    if (rx < 0) rx = 0;
    if (ry < 0) ry = 0;


    if ((uint32_t)rx + rw > TFT_W) rw = TFT_W - rx;
    if ((uint32_t)ry + rh > TFT_H) rh = TFT_H - ry;

    TFT_FillRect((uint16_t)rx, (uint16_t)ry, rw, rh, UI_BG);
}


static void DrawHome(void)
{
    TFT_Fill(UI_BG);

    DrawTextWithShadow(
        CenterX("ROCK PAPER SCISSORS"),
        35,
        "ROCK PAPER SCISSORS",
        UI_PRIMARY,
        UI_TEXT_INV
    );

    DrawTextWithShadow(
        CenterX("CAN YOU BEAT THE ROBOT?"),
        70,
        "CAN YOU BEAT THE ROBOT?",
        UI_PRIMARY,
        UI_TEXT_INV
    );

    DrawCenteredButton(150, "PLAY");
}



static void DrawResultTable(uint8_t you, uint8_t robot)
{
    TFT_Fill(UI_BG);

    DrawTextWithShadow(
        CenterX("RESULT"),
        16,
        "RESULT",
        UI_PRIMARY,
        UI_TEXT_INV
    );


    uint16_t cardW = 135;
    uint16_t cardH = 105;
    uint16_t gap   = 20;

    uint16_t x1 = (TFT_W - (cardW * 2 + gap)) / 2;
    uint16_t x2 = x1 + cardW + gap;
    uint16_t y  = 46;

    TFT_FillRect(x1, y, cardW, cardH, UI_TEXT_INV);
    TFT_FillRect(x2, y, cardW, cardH, UI_TEXT_INV);


    TFT_FillRect(x1, y, cardW, 2, UI_TEXT);
    TFT_FillRect(x1, (uint16_t)(y + cardH - 2), cardW, 2, UI_TEXT);
    TFT_FillRect(x1, y, 2, cardH, UI_TEXT);
    TFT_FillRect((uint16_t)(x1 + cardW - 2), y, 2, cardH, UI_TEXT);

    TFT_FillRect(x2, y, cardW, 2, UI_TEXT);
    TFT_FillRect(x2, (uint16_t)(y + cardH - 2), cardW, 2, UI_TEXT);
    TFT_FillRect(x2, y, 2, cardH, UI_TEXT);
    TFT_FillRect((uint16_t)(x2 + cardW - 2), y, 2, cardH, UI_TEXT);


    DrawTextWithShadow(x1 + 10, y + 10, "YOU",   UI_TEXT, UI_TEXT_INV);
    DrawTextWithShadow(x2 + 10, y + 10, "ROBOT", UI_TEXT, UI_TEXT_INV);

    /* big letters */
    char cYou = (you == ROCK) ? 'R' : (you == PAPER) ? 'P' : 'S';
    char cBot = (robot == ROCK) ? 'R' : (robot == PAPER) ? 'P' : 'S';

    uint8_t scale = 7;
    uint16_t charW = 5 * scale;

    uint16_t iconY = y + 34;
    uint16_t cx1 = x1 + (cardW - charW) / 2;
    uint16_t cx2 = x2 + (cardW - charW) / 2;

    DrawCharScaled(cx1, iconY, cYou, UI_PRIMARY, scale);
    DrawCharScaled(cx2, iconY, cBot, UI_PRIMARY, scale);


    if (you == robot)
        DrawTextWithShadow(CenterX("DRAW"), 165, "DRAW", UI_PRIMARY, UI_TEXT_INV);
    else if (
        (you == ROCK     && robot == SCISSORS) ||
        (you == PAPER    && robot == ROCK)     ||
        (you == SCISSORS && robot == PAPER))
        DrawTextWithShadow(CenterX("YOU WON"), 165, "YOU WON", UI_PRIMARY, UI_TEXT_INV);
    else
        DrawTextWithShadow(CenterX("YOU LOST"), 165, "YOU LOST", UI_PRIMARY, UI_TEXT_INV);


    DrawCenteredButton(210, "HOME");
}


void UI_GameInit(void)
{
    state = GAME_HOME;
    DrawHome();
}



void UI_GameUpdate(void)
{
    uint16_t tx, ty;
    uint32_t now = HAL_GetTick();

    switch (state)
    {


    case GAME_HOME:
    {
        uint8_t t = Touch_Read(&tx, &ty);

        if (t && !touchWasDown)
        {
            countdown = 3;
            timer = now;

            externalGesture = NO_GESTURE;
            externalGestureReady = 0;
            gestureAccepted = 0;

            TFT_Fill(UI_BG);

            DrawTextWithShadow(
                CenterX("GET READY"),
                25,
                "GET READY",
                UI_PRIMARY,
                UI_TEXT_INV
            );

            state = GAME_COUNTDOWN;
        }

        touchWasDown = t;
    }
    break;

/* ============================================================ */
/* COUNTDOWN */
/* ============================================================ */

    case GAME_COUNTDOWN:
    {
        Hand_Thinking();

        if (now - timer >= COUNTDOWN_STEP_MS)
        {
            timer = now;


            uint8_t scale = COUNTDOWN_SCALE;
            uint16_t w = 5 * scale;
            uint16_t x = (TFT_W - w) / 2;
            uint16_t y = COUNTDOWN_Y;


            ClearCountdownDigitArea(x, y, scale);

            char digit = (char)('0' + countdown);
            DrawCharScaled((uint16_t)(x + 2), (uint16_t)(y + 2), digit, UI_TEXT, scale);
            DrawCharScaled(x, y, digit, UI_PRIMARY, scale);


            if (countdown > 1)
            {
                countdown--;
            }
            else
            {
                countdown = 0;

                robotMove = rand() % 3;

                if (robotMove == ROCK)     Hand_Rock();
                if (robotMove == PAPER)    Hand_Paper();
                if (robotMove == SCISSORS) Hand_Scissors();

                TFT_Fill(UI_BG);

                DrawTextWithShadow(
                    CenterX("SHOW YOUR HAND"),
                    170,
                    "SHOW YOUR HAND",
                    UI_TEXT,
                    UI_TEXT_INV
                );

                gestureDeadline = now + HOLD_MS;
                state = GAME_WAIT_HAND;
            }
        }
    }
    break;



    case GAME_WAIT_HAND:
    {
        if (now > gestureDeadline)
        {
            TFT_Fill(UI_BG);

            DrawTextWithShadow(
                CenterX("NO GESTURE"),
                100,
                "NO GESTURE",
                UI_PRIMARY,
                UI_TEXT_INV
            );

            state = GAME_HOME;
            break;
        }

        if (!gestureAccepted && externalGestureReady)
        {
            uint8_t g = externalGesture;

            externalGestureReady = 0;
            externalGesture = NO_GESTURE;

            if (g <= SCISSORS)
            {
                gestureAccepted = 1;
                userMove = g;

                /* go directly to result table */
                DrawResultTable(userMove, robotMove);
                state = GAME_TRY_AGAIN; /* reuse as result buttons handler */
            }
        }
    }
    break;


    case GAME_TRY_AGAIN:
    {
        uint8_t t = Touch_Read(&tx, &ty);

        if (t && !touchWasDown)
        {
            if (ty >= 200)
            {

                DrawHome();
                state = GAME_HOME;
            }
            else
            {

                countdown = 3;
                timer = now;

                externalGesture = NO_GESTURE;
                externalGestureReady = 0;
                gestureAccepted = 0;

                TFT_Fill(UI_BG);
                DrawTextWithShadow(
                    CenterX("GET READY"),
                    25,
                    "GET READY",
                    UI_PRIMARY,
                    UI_TEXT_INV
                );

                state = GAME_COUNTDOWN;
            }
        }

        touchWasDown = t;
    }
    break;


    default:
        DrawHome();
        state = GAME_HOME;
        break;
    }
}
