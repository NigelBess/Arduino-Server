closeAll;
arduino = Arduino('COM5');
arduino.connect();
while true
    msg = input('Message to send arduino: ');
    tic
    arduino.sendMessage(msg);
    response = arduino.getMessage();
    time = toc;
    disp(response);
    disp("message completed in " + string(time)+ " seconds");
    disp(" ");
end
