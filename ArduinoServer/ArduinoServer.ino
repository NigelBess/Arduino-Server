const uint8_t maxMessageLength = 8;//max number of bytes in an incoming message
const byte terminator = 254;//byte representing end of message

const uint8_t serialTimeOutTime = 50;//ms
byte currentMessage[maxMessageLength];//array containing bytes of the incoming message in order

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


//void parse(char* message)
//{
//  char function = message[0];
//  switch(function)
//  {
//    case 0:
//      pinMode(int(message(1),int(message(2))));
//    case 1:
//      pinMode
//  }
//  
//}
