#ifndef Encoder_H
#define Encoder_H

#include "PinObject.h"

class Encoder : public PinObject
{
  protected: 
  const uint8_t maxPinNum = 19;
  long int count = 0;
  uint8_t secondaryPin = 253;
  int8_t direction = 1;
  
  public:
  Encoder(uint8_t pinNum)
  {
    pin = pinNum;
  }
  void setSecondaryPin(uint8_t pinNum)
  {
    secondaryPin = pinNum;
  }
  void setDirection(int8_t dir)
  {
    if (dir==0)
    {
      direction = 0;
      return;
    }
    dir>0 ? direction = 1: direction = -1;
  }
  void interrupt()
  {
    if(secondaryPin>maxPinNum)//invalid pin or no secondary pin selected (non-quadrature encoder)
    {
      count+=direction; 
      return;
    }
    bool a = digitalRead(pin);
    bool b = digitalRead(secondaryPin);
    a^b ? count+=direction : count-=direction;
  }
  int getCount()
  {
    return count;
  }
  void reset()
  {
    count = 0;
  }
};
  
  
  
#endif
