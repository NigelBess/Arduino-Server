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
byte* returnMessage = new byte[maxMessageLength];

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
  Serial.flush();
  Serial.print(msg+String(char(terminator)));
}

byte* getIncomingMessage()
{
  //returns incoming message as a byte array.
  //values after terminator will be zero
  //for example assume an incoming message is 2 bytes: {6 , 7}
  //and in this example maxMessageLength is 5
  //this function returns: {6, 7, [terminator], 0, 0}
  uint8_t index = 0;
  byte* out = new byte[maxMessageLength];
  while(index<maxMessageLength)
  {
    byte currentByte;
    unsigned long int startMessageTime = millis();
    do
    {
      currentByte = Serial.read();
    }
    while((currentByte == 255)&&((millis()-startMessageTime))<serialTimeOutTime);
    if (currentByte == 255) return out;
    out[index] = currentByte;
    index++;
    if(currentByte==terminator) return out;
  }
  return out;
}
void repeat()
{
  byte* incoming = getIncomingMessage();
  for(int i = 0; i < maxMessageLength;i++)
  {
    Serial.print(char((incoming[i])));
    if(incoming[i] == terminator)
    {
      return;
    }
    
  }
  Serial.print(char(terminator));
}


void parse(byte* message)
{
  resetReturnMessage();
  byte function = message[0];
  switch(function)
  {
    case 0://pinMode(pinNumber,pinType)
      setupPin(message[1],message[2]) ? success() : error();
      return;
    case 1://digitalWrite(pinNumber,state)
        writeDigital(message[1],message[2]) ? success() : error();
      return;
    case 2://analogWrite(pinNumber,value)
      writeAnalog(message[1],(message[2])) ? success() : error();
      return;
    case 3://digitalRead(pinNumber)
      if(!validPin(message[1]))
      {
        error();
        return;
      }
      returnMessage[0] = digitalRead(message[1]);
      reply();
      return;
    case 4://analogRead(pinNumber)
      if(!validPin(message[1]))
      {
        error();
      } else
      {
        returnMessage[0] = map(analogRead(message[1]),0,1023,0,maxAnalogValue);
        reply();
      }
      return;
    case 5://servo(pinNumber)
      addServo(message[1]) ? success() : error();
      return;
    case 6://writeServo(pinNumber,value)
      writeServo(message[1],message[2]) ? success() : error();
      return;
    case 7://detachServo(pinNumber)
      detachServo(message[1]) ? success() : error();
      return;
    case 8://addEncoder(interruptPin,optionalSecondaryPin)
      attachEncoder(message[1],message[2]) ? success() : error();
      return;
    case 9: //getEncoderCount(interruptPin)
      putEncoderCountInMessage(message[1]) ? reply() : error();
      return;
    case 10://resetEncoder(interruptPin)
      resetEncoder(message[1]) ? success() : error();
      return;
    case 11://detachEncoder(interruptPin)
      detachEncoder(message[1]) ? success() : error();
      return;
    case 12:
       setEncoderDirection(message[1],message[2]) ? success() : error();
       return;
    case 253://checkConnection
      message[1]==0 ? sendDeviceInfo() : success();
      return;
  
  }
  //if you made it this far, the function byte was invalid
  error();
}
void resetReturnMessage()
{
  //sets the return message to the error message
  returnMessage[0] = errorByte;
  for(int i =1;i<maxMessageLength;i++)
  {
    returnMessage[i] = terminator;
  }
}
void error()
{
  //sets return message to error message and sends to serial com
  resetReturnMessage();
  reply();
}
void success()
{
  //sets return message to success message and sends to serial com
  resetReturnMessage();
  returnMessage[0] = successByte;
  reply();
}
void reply()
{
  Serial.flush();
  for(int i =0;i<maxMessageLength;i++)
  {
    Serial.print(char(returnMessage[i]));
    if (returnMessage[i]==terminator)
    {
      return;
    }
  }
}
bool writeDigital(uint8_t pin, uint8_t state)
{
  if (!validPin(pin)) return false;
  if(!pinAvailable(pin)) return false;
  if (state>maxDigitalState) return false;
  digitalWrite(pin,state);
  return true;
}
bool writeAnalog(uint8_t pin, uint8_t value)
{
  if (!validPin(pin)) return false;
  if(!pinAvailable(pin)) return false;
  if(value>maxAnalogValue) return false;
  analogWrite(pin,analogValue(value));
  return true;
}
bool validPin(uint8_t pin)
{
  return pin<maxPinNum;
}
bool setupPin(uint8_t pin, uint8_t type)
{
  if (!validPin) return false;
  if (!pinAvailable(pin)) return false;
  if (type>maxPinType) return false;
  pinMode(pin,type);
  return true;
}
uint8_t analogValue(uint8_t input)
{
  return map(input,0,maxAnalogValue,0,255);
}
void sendDeviceInfo()
{
  sendMessage(F("Arduino Uno running server code by Nigel Bess: https://github.com/NigelBess/Arduino-Server"));
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

bool addServo(uint8_t pin)
{
  if (!validPin(pin)) return false;
  if (!pinAvailable(pin)) return false;
  uint8_t index = getAvailableSlot((void**)servos,maxServos);
  servos[index] = new ServoObject(pin);
  return true;
}
bool writeServo(uint8_t pin, uint8_t value)
{
  if(!validPin(pin)) return false;
  if(value > 180) return false;
  if(servoAvailable(pin)) return false;
  uint8_t index = getServoIndexByPin(pin);
  if (index == 255) return false;
  (*servos[index]).write(value);
  return true;
}
bool detachServo(uint8_t pin)
{
  if(!validPin(pin)) return false;
   if(servoAvailable(pin)) return false;
    uint8_t index = getServoIndexByPin(pin);
    while(index != 255)
    {
      (*servos[index]).detach();//make arduino stop using the pin for servo communication
      delete servos[index];//deallocate memory where the servo object was stored
      servos[index] = NULL;//dereference the memory
      index = getServoIndexByPin(pin);//check if there is another servo that uses this pin
   }
  return true; 
}
bool attachEncoder(uint8_t pin,uint8_t secondaryPin)
{
  int8_t interruptPin = digitalPinToInterrupt(pin);
  if (interruptPin<0) return false;// not a valid interrupt pin
  if(!setupPin(pin,INPUT)) return false;//try to set up the interrupt pin as input. if fails return false
  
  bool quadratureEncoder = validPin(secondaryPin);//is it a quadrature encoder? (did the user specify a valid secondary pin)
  if(quadratureEncoder && !setupPin(secondaryPin,INPUT)) return false;

  
  uint8_t index = getAvailableSlot((void**)encoders,numInterruptPins);
  encoders[index] = new Encoder(pin);
  if(quadratureEncoder) (*encoders[index]).setSecondaryPin(secondaryPin);
  switch (index)
  {
    case 0:
      attachInterrupt(interruptPin,encoder0Interrupt,FALLING);
      break;
    case 1:
      attachInterrupt(interruptPin,encoder1Interrupt,FALLING);
      break;
  }
  return true;
}
bool validEncoder(uint8_t pin)
{
  return validPin && !encoderAvailable(pin);
}
bool putEncoderCountInMessage(uint8_t pin)
{
  if(!validEncoder(pin)) return false;
  uint8_t index = getEncoderIndexByPin(pin);
  int count = (*encoders[index]).getCount();
  putStringInReturnMessage(serialize(count),0);
  return true; 
}
bool resetEncoder(uint8_t pin)
{
  if(!validEncoder(pin)) return false;
  uint8_t index = getEncoderIndexByPin(pin);
  (*encoders[index]).reset();
  return true;
}
bool detachEncoder(uint8_t pin)
{
  if(!validEncoder(pin)) return false;
  uint8_t index = getEncoderIndexByPin(pin);
  detachInterrupt(index);//index (0 or 1) matches interruptPin
  delete encoders[index];//deallocate memory for encoder object
  encoders[index] = NULL;//dereference 
  return true;
}
bool setEncoderDirection(uint8_t pin, uint8_t direction)
{
  if(!validEncoder(pin)) return false;
  if(direction>2) return false;//direction can be 0 (no count), 1 (count positive) or (2 count negative)
  //convert uint8 to int8 with 2 mapping to -1
  int8_t intDir = int8_t(direction);
  if (intDir == 2) intDir = -1;
  uint8_t index = getEncoderIndexByPin(pin);
  (*encoders[index]).setDirection(intDir);
  return true;
}
void putStringInReturnMessage(String str, uint8_t index)
{
  for (int i = 0; i<str.length(); i++)
  {
    returnMessage[index+i] = str[i];
  }
}
String serialize(int input)
{
  //obviously each byte is 8 bits, but we cant use 255 or 254 because those are used for a null message and the terminator
  //so we have to use one less bit per byte when transferring large numbers over multiple bytes
  uint8_t totalBits = sizeof(input)*8;
  uint8_t bytesNeeded = totalBits/numBitsPerByte + int((totalBits%numBitsPerByte)>0);
  String out;
  int remainder = 1<<numBitsPerByte;
  input<0 ? out+= char(1) : out+=char(0);
  input = abs(input);
  for (int i = 0; i<bytesNeeded;i++)
  {
    uint8_t shift = numBitsPerByte*(i+1);
    remainder = input%(1<<shift);
    out += char(remainder>>(shift-numBitsPerByte));//little endian
    input -= remainder;
  }
  return out;
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
void debug(int in)
{
  Serial.print(char(in));
}
