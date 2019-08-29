clearAll;
arduino = Arduino('COM4');
arduino.connect();
startTime = arduino.getTime();
time = startTime;
totalTime = 24;%hours
totalTime = totalTime*60;%minute
totalTime = totalTime*60;%seconds
try
arduino.attachServo(9);
arduino.encoder(2,6);
i = 0;
while (time - startTime) < totalTime
    tic
    i = i+1;
    arduino.pinMode(4,"OUTPUT");%function 0
    arduino.digitalWrite(4,"HIGH");%function 1
     arduino.pinMode(11,"OUTPUT");
    arduino.analogWrite(11,128);%function 2
    arduino.pinMode(5,"INPUT");
    arduino.digitalRead(5);%function 3
    arduino.pinMode(14,"INPUT");
    arduino.analogRead(14);%function 4
    
    arduino.attachServo(10);%function 5
    arduino.writeServo(9,120);%function 6
    arduino.writeServo(10,120);
    arduino.detachServo(10);%function7
    
    arduino.encoder(3,7);%function 8
    arduino.readEncoder(2);%function 9
    arduino.resetEncoder(2);%function 10
    arduino.detachEncoder(3);%function 11

    arduino.setEncoderDirection(2,1);%function 12
    arduino.checkConnection();%function 253
    disp("completed cycle "+i+" in " + string(toc) + " seconds.")
    time = arduino.getTime();
end
catch e
    arduino.clearPort();
    disp("time to failure " + string((time-startTime)) + " seconds");
    rethrow(e)
end
