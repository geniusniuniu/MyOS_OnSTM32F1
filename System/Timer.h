#ifndef __Timer_H
#define __Timer_H

#define FLAG_STRAIGHT       1
#define FLAG_LEFT_1         2
#define FLAG_RIGHT_1        3
#define FLAG_LEFT_2         4
#define FLAG_RIGHT_2        5

extern uint8_t retVal1;

void TIM4_Int_Init(u16 arr,u16 psc);


#endif
