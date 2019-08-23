const uint8_t maxMessageLength = 10;//max number of bytes in an incoming message
const byte terminator = 0;//incoming byte representing end of message

byte currentMessage[maxMessageLength];//array containing bytes of the incoming message in order

void setup() 
{
  Serial.begin(250000);
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
  Serial.print(msg+" ");
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
    while( Serial.available()==0){}
    byte currentByte = Serial.read();
    out[index] = currentByte;
    if(currentByte==terminator)
    {
      break;
    }
  }
  return out;
}
void repeat()
{
  byte* incoming = getIncomingMessage();
  for(int i = 0; i < maxMessageLength;i++)
  {
    Serial.print(incoming[i]);
    if(incoming[i] == terminator)
    {
      return;
    }
  }
  Serial.print(" ");
}
