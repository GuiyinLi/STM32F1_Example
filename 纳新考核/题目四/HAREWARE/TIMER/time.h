#ifndef _timer_h
#define _timer_h
#include "sys.h"    //这里面已经引入了顶层头文件

void TIM5_Int_Init(u16 arr,u16 psc);
void TIM5_PWM_Init(u16 arr,u16 psc);

#endif

