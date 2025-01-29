#ifndef __SERVO_H
#define __SERVO_H

#define Servo_MAX  2200
#define Servo_MIN  1000

#define SERVO_DISTRICT(value) ((value) < (Servo_MIN) ? (Servo_MIN) : ((value) > (Servo_MAX) ? (Servo_MAX) : (value)))

void Servo_Init(void);
uint16_t Servo_SetCompare(uint16_t Compare);

#endif
