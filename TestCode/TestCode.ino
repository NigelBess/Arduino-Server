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
    String in = Serial.readString();
    Serial.println(in[0]);
    Serial.println(in[1]);
    
  } 

}
