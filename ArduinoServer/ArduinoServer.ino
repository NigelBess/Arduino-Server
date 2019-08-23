const uint8_t maxMessageLength = 10;//max number of bytes in an incoming message
const char terminator = 127;//byte representing end of message

int serialTimeOutTime = 50;//ms
byte currentMessage[maxMessageLength];//array containing bytes of the incoming message in order

void setup() 
{
  Serial.begin(250000);
  Serial.flush();
  Serial.setTimeout(serialTimeOutTime);
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
  Serial.print(msg+terminator);
}

char* getIncomingMessage()
{
  //returns incoming message as a byte array.
  //values after terminator will be zero
  //for example assume an incoming message is 2 bytes: {6 , 7}
  //and in this example maxMessageLength is 5
  //this function returns: {6, 7, [terminator], 0, 0}
  uint8_t index = 0;
  char* out = new char[maxMessageLength];
  while(index<maxMessageLength)
  {
    char currentByte;
    unsigned long int startMessageTime = millis();
    do
    {
      currentByte = Serial.read();
    }
    while((currentByte == -1)&&((millis()-startMessageTime))<serialTimeOutTime);
    if (currentByte == -1) return out;
    out[index] = currentByte;
    index++;
    if(currentByte==terminator) return out;
  }
  return out;
}
void parse()
{
  char* incoming = getIncomingMessage();
  
}
void repeat()
{
  char* incoming = getIncomingMessage();
  for(int i = 0; i < maxMessageLength;i++)
  {
    Serial.print(incoming[i]);
    if(incoming[i] == terminator)
    {
      return;
    }
    
  }
  Serial.print(terminator);
}
