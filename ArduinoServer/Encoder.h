#ifndef Encoder_H
#define Encoder_H

#include "PinObject.h"

class Encoder : public PinObject
{
  protected: 
  
  int count;
  
  public:
  Encoder(uint8_t pinNum)
  {
    pin = pinNum;
  }
  void detach()
  {
    servo.detach();
    pin = 255;
  }
  void write(uint8_t pos)
  {
    servo.write(pos);
  }
};
  
  
  
#endif
