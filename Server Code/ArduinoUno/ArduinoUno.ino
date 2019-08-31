#include "PinObject.h"
#include "ServoObject.h"
#include "Encoder.h"

const uint8_t maxMessageLength = 8;//max number of bytes in an incoming message

const uint8_t numBitsPerByte = 7;//useable bits per byte for sending large numbers over multiple bytes
//obviously each byte is 8 bits, but we cant use values of 255 or 254 because those are used for a null message and the terminator
  //so we have to use one less bit per byte when transferring large numbers over multiple bytes

const byte nullByte = 255;//byte representing a lack of available Serial input
const byte terminator = 254;//byte representing end of message
const byte errorByte = 253;//used in the return message to signify an error
const byte successByte = 0;//used in the return message to signify successful execution of a function
const byte maxAnalogValue = 252;//corresponds to 255 in arduino's analog read/write functions


const uint8_t startDelay = 100;//ms
const uint8_t serialTimeOutTime = 50;//ms
byte* message = new byte[maxMessageLength];//incoming message buffer

const uint8_t maxPinNum = 19;
const uint8_t maxPinType = 2;
const uint8_t maxDigitalState = 1;
const uint8_t numInterruptPins = 2;

const uint8_t maxServos = maxPinNum +1;
ServoObject** servos = new ServoObject*[maxServos]{NULL};
Encoder** encoders = new Encoder*[numInterruptPins]{NULL};

void setup() 
{
  Serial.begin(250000);
  
  Serial.setTimeout(serialTimeOutTime);
  delay(startDelay);
  sendDeviceInfo();
  
}

void loop() 
{
  if(Serial.available()>0)
  {
    parse(getIncomingMessage());
  } 
}
void sendMessage(String msg)
{
  Serial.print(msg+String(char(terminator)));
}
void sendMessage(int input)
{
  //obviously each byte is 8 bits, but we cant use 255 or 254 because those are used for a null message and the terminator
  //so we have to use one less bit per byte when transferring large numbers over multiple bytes
  uint8_t totalBits = sizeof(input)*8;
  uint8_t bytesNeeded = totalBits/numBitsPerByte + int((totalBits%numBitsPerByte)>0);
  uint8_t firstValue = uint8_t(int(input<0));
  Serial.print(char(firstValue));
  input = abs(input);
  for (uint8_t i = 0; i<bytesNeeded;i++)
  {
    uint8_t shift = (i+1)*numBitsPerByte;
    uint32_t remainder = abs(input)%int(pow(2,shift));
    Serial.print(char(uint8_t(remainder>>(shift-numBitsPerByte))));//little endian
    input -= remainder;
  }
  Serial.print(char(terminator));
}
void error(String msg)
{
  Serial.print(char(errorByte));
  sendMessage(msg);
}
void invalidPinError(uint8_t pin)
{
  error("Pin " + String(pin) + " is invalid. Valid pins are 0-19.");
}
void unavailablePinError(uint8_t pin)
{
  error("Pin " + String(pin) + " is already in use by an encoder or servo.");
}
void findServoError(uint8_t pin)
{
  findError("servo",pin);
}
void findEncoderError(uint8_t pin)
{
  findError("encoder interrupt pin",pin);
}
void findError(String type, uint8_t pin)
{
  error("No "+ type+ " found at pin "+ String(pin));
}
void runtimeError()
{
  error(F("The Arduino has encountered a runtime error (this isn't supposed to happen). If you are reading this message, there is a bug in the Arduino server code. Probably a buffer error or stack overflow."));
}
void success()
{
  sendMessage(byte(0));
}



byte* getIncomingMessage()
{
  for (int i = 0;i<maxMessageLength;i++)
  {
    message[i] = terminator;
  }
  uint8_t index = 0;
  while(index<maxMessageLength)
  {
    byte currentByte;
    unsigned long int startMessageTime = millis();
    do
    {
      currentByte = Serial.read();
    }
    while((currentByte == 255)&&((millis()-startMessageTime))<serialTimeOutTime);
    if (currentByte == 255) return message;
    message[index] = currentByte;
    index++;
    if(currentByte==terminator) return message;
  }
  return message;
}



void parse(byte* message)
{
  byte function = message[0];
  switch(function)
  {
    case 0://pinMode(pinNumber,pinType)
      Try(setupPin(message[1],message[2]));
      return;
    case 1://digitalWrite(pinNumber,state)
       Try(writeDigital(message[1],message[2]));
      return;
    case 2://analogWrite(pinNumber,value)
      Try(writeAnalog(message[1],(message[2])));
      return;
    case 3://digitalRead(pinNumber)
      Try(readDigital(message[1]));
      return;
    case 4://analogRead(pinNumber)
      Try(readAnalog(message[1]));
      return;
    case 5://servo(pinNumber)
      Try(addServo(message[1]));
      return;
    case 6://writeServo(pinNumber,value)
      Try(writeServo(message[1],message[2]));
      return;
    case 7://detachServo(pinNumber)
      Try(detachServo(message[1]));
      return;
    case 8://addEncoder(interruptPin,optionalSecondaryPin)
      Try(attachEncoder(message[1],message[2]));
      return;
    case 9: //getEncoderCount(interruptPin)
      Try(readEncoder(message[1]));
      return;
    case 10://resetEncoder(interruptPin)
      Try(resetEncoder(message[1]));
      return;
    case 11://detachEncoder(interruptPin)
      Try(detachEncoder(message[1]));
      return;
    case 12:
       Try(setEncoderDirection(message[1],message[2]));
       return;
    case 253://checkConnection
      message[1]==0 ? sendDeviceInfo() : success();
      return;
  }
  //if you made it this far, the function byte was invalid
  error("Invalid function parameter.");
}
void Try(bool functionOutput)
{
  if (!functionOutput) runtimeError();
  //all paths of every function should return true. If the function returns false it encoutered an error.
}

bool setupPin(uint8_t pin, uint8_t type)
{
  if (!validPin(pin))
  {
    invalidPinError(pin);
    return true;
  }
  if (!pinAvailable(pin)) 
  {
    unavailablePinError(pin);
    return true;
  }
  if (type>maxPinType)
  {
    error(String(type) + " does not name a valid pin type. Valid types are 0, 1 or 2");
    return true;
  }
  pinMode(pin,type);
  success();
  return true;
}

bool writeDigital(uint8_t pin, uint8_t state)
{
  if (!validPin(pin))
  {
    invalidPinError(pin);
    return true;
  }
  if (!pinAvailable(pin)) 
  {
    unavailablePinError(pin);
    return true;
  }
  
  if (state>maxDigitalState) 
  {
    error(String(state) + " is not a valid pin state. valid states are 0 (LOW) or 1(HIGH)");
    return true;
  }
  digitalWrite(pin,state);
  success();
  return true;
}
bool writeAnalog(uint8_t pin, uint8_t value)
{
  if (!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  if(!pinAvailable(pin))
  {
    unavailablePinError(pin);
    return true;
  }
  if(value>maxAnalogValue) 
  {
    error(String(value)+ " is outside the acceptable range of 0-"+String(maxAnalogValue));
  }
  analogWrite(pin,analogValue(value));
  success();
  return true;
}
uint8_t analogValue(uint8_t input)
{
  return map(input,0,maxAnalogValue,0,255);
}


bool readDigital(uint8_t pin)
{
  if(!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  sendMessage(String(char(digitalRead(pin))));
  return true;
}
bool readAnalog(uint8_t pin)
{
  if(!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  sendMessage(int(analogRead(pin)));
  return true;
}



bool addServo(uint8_t pin)
{
  if(!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  if(!pinAvailable(pin))
  {
    unavailablePinError(pin);
    return true;
  }
  uint8_t index = getAvailableSlot((void**)servos,maxServos);
  servos[index] = new ServoObject(pin);
  success();
  return true;
}
bool writeServo(uint8_t pin, uint8_t value)
{
  if(!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  if(value > 180)
  {
    error(String(value) + " degrees outside of range 0-180");
    return true;
  }
  if(servoAvailable(pin))
  {
    findServoError(pin);
    return true;
  }
  uint8_t index = getServoIndexByPin(pin);
  (*servos[index]).write(value);
  success();
  return true;
}
bool detachServo(uint8_t pin)
{
  if(!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  if(servoAvailable(pin))
  {
    findServoError(pin);
    return true;
  }
  uint8_t index = getServoIndexByPin(pin);
  while(index != 255)
  {
    (*servos[index]).detach();//make arduino stop using the pin for servo communication
    delete servos[index];//deallocate memory where the servo object was stored
    servos[index] = NULL;//dereference the memory
    index = getServoIndexByPin(pin);//check if there is another servo that uses this pin
 }
 success();
  return true; 
}
bool attachEncoder(uint8_t pin,uint8_t secondaryPin)
{
  int8_t interruptPin = digitalPinToInterrupt(pin);
  if (interruptPin<0)// not a valid interrupt pin
  {
    error("Pin " + String(pin) + " is not an interrupt pin on this Arduino.");
  }
  if(!validPin(pin)) 
  {
    invalidPinError(pin);
    return true;
  }
  if(!pinAvailable(pin))
  {
    unavailablePinError(pin);
    return true;
  }
  pinMode(pin,INPUT);
  
  bool quadratureEncoder = validPin(secondaryPin);//is it a quadrature encoder? (did the user specify a valid secondary pin)
  if(quadratureEncoder)
  {
    if(!pinAvailable(secondaryPin)) 
    {
      unavailablePinError(secondaryPin);
      return true;
    }
    pinMode(secondaryPin,INPUT);
  }

  uint8_t index = getAvailableSlot((void**)encoders,numInterruptPins);
  encoders[index] = new Encoder(pin);
  if(quadratureEncoder) (*encoders[index]).setSecondaryPin(secondaryPin);
  switch (index)
  {
    case 0:
      attachInterrupt(interruptPin,encoder0Interrupt,CHANGE);
      break;
    case 1:
      attachInterrupt(interruptPin,encoder1Interrupt,CHANGE);
      break;
  }
  success();
  return true;
}
bool readEncoder(uint8_t pin)
{
  if(!validEncoder(pin))
  {
    findEncoderError(pin);
    return true;
  }
  uint8_t index = getEncoderIndexByPin(pin);
  int count = (*encoders[index]).getCount();
  sendMessage(count);
  return true; 
}

bool resetEncoder(uint8_t pin)
{
  if(!validEncoder(pin))
  {
    findEncoderError(pin);
    return true;
  }
  uint8_t index = getEncoderIndexByPin(pin);
  (*encoders[index]).reset();
  if (index>numInterruptPins-1)
  {
    error(F("Fncoder exists but failed to find index. (This is a problem with the server side code)"));
    return true;
  }
  success();
  return true;
}

bool detachEncoder(uint8_t pin)
{
  if(!validEncoder(pin))
  {
    findEncoderError(pin);
    return true;
  }
  uint8_t index = getEncoderIndexByPin(pin);
  detachInterrupt(index);//index (0 or 1) matches interruptPin
  delete encoders[index];//deallocate memory for encoder object
  encoders[index] = NULL;//dereference 
  success();
  return true;
}
bool setEncoderDirection(uint8_t pin, uint8_t direction)
{
  if(!validEncoder(pin))
  {
    findEncoderError(pin);
    return true;
  }
  if(direction>2)//direction can be 0 (no count), 1 (count positive) or (2 count negative)
  {
    error(String(direction) + " is not a valid encoder direction. Valid directions: 0 (no count), 1 (count positive) or (2 count negative)");
  }
  //convert uint8 to int8 with 2 mapping to -1
  int8_t intDir = int8_t(direction);
  if (intDir == 2) intDir = -1;
  uint8_t index = getEncoderIndexByPin(pin);
  (*encoders[index]).setDirection(intDir);
  success();
  return true;
}


bool pinAvailableInObjectArray(uint8_t pin, PinObject** objectSlots,uint8_t size)
{
  for(int i = 0;i<size;i++)
  {
    if(objectSlots[i]!=NULL && ((*objectSlots[i]).getPin()==pin))
    {
      return false;
    }
  }
  return true;
}
uint8_t getAvailableSlot(void** objectSlots, uint8_t size)
{
  for(int i = 0;i<size;i++)
  {
    if(objectSlots[i]==NULL)
    {
      return i;
    }
  }
}
uint8_t getPinObjectByPin(uint8_t pin, PinObject** objectSlots,uint8_t size)
{
  for(int i = 0;i<size;i++)
  {
    if(objectSlots[i]!=NULL && ((*objectSlots[i]).getPin()==pin))
    {
      return i;
    }
  }
  return 255;
}
uint8_t getServoIndexByPin(uint8_t pin)
{
  return getPinObjectByPin(pin,(PinObject**)servos,maxServos);
}
uint8_t getEncoderIndexByPin(uint8_t pin)
{
  return getPinObjectByPin(pin,(PinObject**)encoders,numInterruptPins);
}
bool servoAvailable(uint8_t pin)
{
 return pinAvailableInObjectArray(pin,(PinObject**)servos,maxServos);
}
bool encoderAvailable(uint8_t pin)
{
  return pinAvailableInObjectArray(pin,(PinObject**)encoders,numInterruptPins);
}
bool pinAvailable(uint8_t pin)
{
  return servoAvailable(pin) && encoderAvailable(pin);
}

bool validEncoder(uint8_t pin)
{
  return validPin && !encoderAvailable(pin);
}








void encoder0Interrupt()
{
  if(encoders[0] == NULL) return;
  (*encoders[0]).interrupt();
}

void encoder1Interrupt()
{
  if(encoders[1] == NULL) return;
  (*encoders[1]).interrupt();
}


bool validPin(uint8_t pin)
{
  return pin<maxPinNum;
}

void sendDeviceInfo()
{
  sendMessage(F("Arduino Uno running server code by Nigel Bess: https://github.com/NigelBess/Arduino-Server"));
}
void debug(int in)
{
  Serial.print(char(in));
}
