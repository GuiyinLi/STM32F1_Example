#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define relay PCout(13)
#define led1 PBout(0)

void LED_Init(void);

#endif

