#ifndef TOUCH_H
#define TOUCH_H

#include "main.h"
#include <stdbool.h>

bool Touch_Read(uint16_t *x, uint16_t *y);
void Touch_Reset(void);

#endif
