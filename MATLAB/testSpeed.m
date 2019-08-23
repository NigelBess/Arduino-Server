
try
closeAll;
port = 'COM4';
com = serial(port);
com.BaudRate = 250000;
com.Terminator = 127;
fopen(com);
flushinput(com);
disp(native2unicode(fgetl(com)));
disp('connected')
tic
n = 1E2;
for i = 1:n
    fwrite(com,[33,126,com.Terminator]);
    disp(native2unicode(fgetl(com)));
end
disp("time per com: "+num2str(toc/n));
closePorts;
catch e
    closePorts;
    rethrow(e);
end