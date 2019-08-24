closeAll;
arduino = Arduino('COM4');
arduino.connect();
while true
    msg = input('Message to send arduino: ');
    arduino.sendMessage(msg);
    disp(arduino.getMessage())
end
