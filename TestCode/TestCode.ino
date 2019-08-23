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
    Serial.print("int(A1): ");
    Serial.println(int(A1));
     Serial.print("int(OUTPUT): ");
    Serial.println(int(OUTPUT));
     Serial.print("int(INPUT_PULLUP): ");
    Serial.println(int(INPUT_PULLUP));
    
  } 

}
