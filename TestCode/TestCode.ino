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
    int val = int(in-'0');
     Serial.print("as int: ");
    Serial.println(val);
    Serial.print("as interrupt: ");
    Serial.println(digitalPinToInterrupt(val));
    
  } 

}
