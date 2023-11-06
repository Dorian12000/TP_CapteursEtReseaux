#ifndef __MOTOR_H
#define __MOTOR_H

void motorInit(void);
void motorSetPosition(uint8_t positionAngle, uint8_t positionSign);
void motorSetPositionDenpendingTemperature(void);

#endif