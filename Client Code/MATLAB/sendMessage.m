function sendMessage(comDevice,byteArray,terminator)
fwrite(comDevice,[byteArray,terminator]);
end