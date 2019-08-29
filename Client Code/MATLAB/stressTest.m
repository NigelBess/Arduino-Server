clearAll;
arduino = Arduino('COM4');
arduino.connect();
startTime = arduino.getTime();
time = startTime;
total = 24;%hours
totalTime = totalTime*60;%minute
totalTime = totalTime*60;%seconds
try
arduino.attachServo(9);
arduino.attachServo(10);
arduino.encoder(2,6);
i = 0;
while (time - startTime) < totalTime
    tic
    i = i+1;
    arduino.pinMode(5,"INPUT");
    arduino.digitalRead(5);
    arduino.pinMode(11,"OUTPUT");
    arduino.analogWrite(11,128);
    arduino.pinMode(4,"OUTPUT");
    arduino.digitalWrite(4,"HIGH");
    arduino.pinMode(14,"INPUT");
    arduino.analogRead(14);
    
    arduino.writeServo(9,120);
    arduino.writeServo(10,120);
    arduino.detachServo(10);
    arduino.attachServo(10);
    arduino.encoder(3,7);
    arduino.detachEncoder(3);
    arduino.readEncoder(2);
    arduino.resetEncoder(2);
    arduino.setEncoderDirection(2,1);
    disp("completed cycle "+i+" in " + string(toc) + " seconds.")
    time = arduino.getTime();
end
catch e
    arduino.clearPort();
    disp("time to failure " + string((time-startTime)) + " seconds");
    rethrow(e)
end
