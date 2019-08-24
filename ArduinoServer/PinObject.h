#ifndef PinObject_H
#define PinObject_H


class PinObject
{
  protected: 
  
  uint8_t pin = 253;
  
  public:
  uint8_t getPin()
  {
    return pin;
  }
};
  
  
  
#endif
