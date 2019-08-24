#ifndef ServoObject_H
#define ServoObject_H

#include <Servo.h>

class ServoObject
{
  protected: 
  
  uint8_t pin = 253;
  Servo servo;
  
  public:
  ServoObject(uint8_t pin)
  {
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
  uint8_t getPin()
  {
    return pin;
  }
};
  
  
  
#endif
