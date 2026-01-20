#ifndef HAND_H
#define HAND_H

#include <stdint.h>

/* ===== servo channels ===== */

#define SERVO_THUMB     0
#define SERVO_INDEX     1
#define SERVO_MIDDLE    2
#define SERVO_RINGP     3   // ring + pinky together
#define SERVO_THUMB_SIDE  5
#ifndef GESTURE_H
#define GESTURE_H
#ifndef HAND_H
#define HAND_H


void Hand_Rock(void);
void Hand_Paper(void);
void Hand_Scissors(void);


#endif

#define ROCK     0
#define PAPER    1
#define SCISSORS 2

#endif



/* ===== angles ===== */

// ===== individual finger angles =====

// thumb
#define THUMB_OPEN      0
#define THUMB_CLOSED    160

// index
#define INDEX_OPEN     115
#define INDEX_CLOSED   0
// middle
#define MIDDLE_OPEN     10
#define MIDDLE_CLOSED   160

#define RINGP_OPEN     110
#define RINGP_CLOSED   0

#define THUMB_LEFT   350
#define THUMB_RIGHT  200


#define THUMB_SIDE_OUT   0   // rock / paper
#define THUMB_SIDE_IN    1   // grip



/* ===== gestures ===== */
float FingerAngle(uint8_t finger, float t);
void Hand_Rock(void);
void Hand_Paper(void);
void Hand_Scissors(void);
void Hand_Thinking(void);
void Hand_RandomGesture(void);
void Hand_TestLoop(void);
void Hand_VisionMirror(uint8_t gesture);

#endif
