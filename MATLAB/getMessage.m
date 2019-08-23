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