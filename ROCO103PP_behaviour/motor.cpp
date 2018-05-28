#include "motor.h"

Motor::Motor(PinName pinName1, PinName pinName2) : pin1(pinName1), pin2(pinName2)
{
}
void Motor::Fwd(float duty)
{ 
    this->pin1 = 0.0f;
    this->pin2 = duty;
}
void Motor::Rev(float duty)
{
    this->pin1 = duty;
    this->pin2 = 0.0f;
}
void Motor::Period_in_ms(int msPeriod)
{
    this->pin1.period_ms(msPeriod);
    this->pin2.period_ms(msPeriod);
}
