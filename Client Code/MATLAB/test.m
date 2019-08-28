clearAll;
arduino = Arduino('COM5');
arduino.connect();
pin = "D7";
arduino.pinMode(pin,"INPUT_PULLUP");
i = 0;
while true
    i = i+1;
    tic
    response = arduino.digitalRead(pin);
    time = toc;
    if time>0
    disp(response);
    disp("message "+string(i)+" completed in " + string(time)+ " seconds");
    disp(" ");
    end
end
