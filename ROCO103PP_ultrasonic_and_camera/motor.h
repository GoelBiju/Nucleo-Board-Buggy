#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "mbed.h"
class Motor
{
public:
    Motor(PinName pinName1, PinName pinName2);
    void Fwd(float time);
    void Rev(float time);
    void Period_in_ms(int msPeriod);
private:
    PwmOut pin1;
    PwmOut pin2;
};
#endif
