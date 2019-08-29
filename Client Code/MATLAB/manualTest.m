clearAll;
arduino = Arduino('COM4');
arduino.connect();
while true
    msg = input('Message to send arduino: ');
    tic
    try
        response = arduino.sendMessageReliable(msg);
    catch e
        disp(e.message)
        response = "";
    end
    time = toc;
    disp(response);
    disp("message completed in " + string(time)+ " seconds");
    disp(" ");
end
