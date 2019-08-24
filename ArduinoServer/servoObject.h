#ifndef ServoObject_H
#define ServoObject_H

#include "PinObject.h"
#include <Servo.h>

class ServoObject : public PinObject
{
  protected: 
  
  Servo servo;
  
  public:
  ServoObject(uint8_t pinNum)
  {
    pin = pinNum;
    servo.attach(pin);
  }
  void detach()
  {
    servo.detach();
  }
  void write(uint8_t pos)
  {
    servo.write(pos);
  }
};
  
  
  
#endif
