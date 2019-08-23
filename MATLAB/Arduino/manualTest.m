closeAll;
arduino = Arduino('COM4');
while true
    msg = input('Message to send arduino: ');
    arduino.sendMessage(msg);
    disp(arduino.getMessage())
end
