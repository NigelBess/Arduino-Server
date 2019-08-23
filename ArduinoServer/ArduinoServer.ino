const uint8_t maxMessageLength = 8;//max number of bytes in an incoming message

const byte nullByte = 255;//byte representing a lack of available Serial input
const byte terminator = 254;//byte representing end of message
const byte errorByte = 253;//used in the return message to signify an error
const byte successByte = 0;//used in the return message to signify successful execution of a function



const uint8_t serialTimeOutTime = 50;//ms
byte* returnMessage = new byte[maxMessageLength];

const uint8_t maxPinNum = 19;
const uint8_t maxPinType = 2;
const uint8_t maxDigitalPinState = 1;

void setup() 
{
  Serial.begin(250000);
  
  Serial.setTimeout(serialTimeOutTime);
  Serial.flush();
  sendMessage("on");
}

void loop() 
{
  if(Serial.available()>0)
  {
    repeat();
  } 
}
void sendMessage(String msg)
{
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
      if(message[1]>maxPinNum || message[2]>maxPinType)//pin out of range or pinType not defined
      {
        error();
        return;
      }
      pinMode(uint8_t(message[1]),uint8_t(message[2]));
      success();
      return;
    case 1://digitalWrite(pinNumber,state)
    return;
  }
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
