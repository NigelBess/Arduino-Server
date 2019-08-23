

try
closeAll;

port = 'COM4';
com = serial(port);
com.BaudRate = 250000;
terminator = 254;
fopen(com);
flushinput(com);
pause(0.2);
disp(native2unicode(getMessage(com,terminator)));
disp('connected')
tic
n = 1E1;
for i = 1:n
    fwrite(com,[252,101,126,terminator]);
    disp((getMessage(com,terminator)));
    disp(i);
end
disp("time per com: "+num2str(toc/n));
closePorts;
catch e
    closePorts;
    rethrow(e);
end

function out = getMessage(comDevice,terminatorByte)
    maxMessageLength = 8;
    messageArray = zeros(1,maxMessageLength);
    index = 0;
    while (index<=(maxMessageLength-1))
        index = index+1;   
        while (~comDevice.BytesAvailable)
        end
        newByte = fread(comDevice,1);
        if newByte == terminatorByte
            index = index-1;
            break;
        end
        messageArray(index) = newByte;
    end
    out = messageArray(1:index);
end