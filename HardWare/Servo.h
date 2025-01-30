#ifndef __SERVO_H
#define __SERVO_H

#define Servo_MAX  1600
#define Servo_MIN  800

#define SERVO_DISTRICT(value) ((value) < (Servo_MIN) ? (Servo_MIN) : ((value) > (Servo_MAX) ? (Servo_MAX) : (value)))

void Servo_Init(void);
uint16_t Servo_SetCompare(uint16_t Compare);

#endif
