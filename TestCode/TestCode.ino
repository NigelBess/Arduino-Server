const uint8_t serialTimeOutTime = 50;//ms

void setup() 
{
  Serial.begin(250000);
  Serial.flush();
  Serial.setTimeout(serialTimeOutTime);
  Serial.println("on");
}

void loop() 
{
  if(Serial.available()>0)
  {
    char in = Serial.read();
    Serial.print("int(char): ");
    Serial.println(int(in));
    Serial.print("(int)char: ");
    Serial.println((int)in);
    Serial.print("byte(char): ");
    Serial.println(byte(in));
    Serial.print("(byte)char: ");
    Serial.println((byte)in);
    
  } 

}
