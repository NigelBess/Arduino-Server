classdef Arduino < handle
    properties(Constant)
        baudRate = 250000;
        terminator = 254;
        messageBufferSize = 512;
        serialOpenDelay = 2;
        serialTimeOut = 2;
    end
    properties(Access = private)
        comPort;
        messageBuffer;
        portName;
    end
    
    methods(Access = public)
        function obj = Arduino(port)
            portName = port;
            obj.messageBuffer = zeros(1,obj.messageBufferSize);
            obj.comPort = serial(port);
            obj.comPort.BaudRate = obj.baudRate;
            fopen(obj.comPort);
            flushinput(obj.comPort);
            pause(obj.serialOpenDelay);
            disp("Connected to " + obj.getMessageAsText());
        end
        function sendMessage(obj,msg)
            %sanitize input
            for i = 1:numel(msg)
                msg(i) = floor(msg(i));%only allow integers
                
                % we will be sending each array element as an individual byte, so all values
                % must be between 0 and 255.
                if (msg(i)>255||msg(i)<0)
                    msg(i) = 255;
                end
            end
            %values of 255 get ignored by the arduino so we will delete
            %those
            msg(msg==255) = [];
            fwrite(obj.comPort,[msg,obj.terminator]);
        end
        function out = getMessage(obj)
            index = 0;
            startTime = obj.getTime();
            while (index<=(obj.messageBufferSize-1))
                index = index+1;   
                while (~obj.comPort.BytesAvailable)
                    if((obj.getTime()-startTime)>obj.serialTimeOut)
                        obj.clearPort();
                        error('Arduino has timed out or disconnected.');
                    end
                end
                newByte = fread(obj.comPort,1);
                if newByte == obj.terminator
                    index = index-1;
                    break;
                end
                obj.messageBuffer(index) = newByte;
            end
            out = obj.messageBuffer(1:index);
        end
        function out = getMessageAsText(obj)
            out = string(native2unicode(obj.getMessage()));
        end
        function out = getTime(obj)
            c = clock;
            out = c(6);
        end
        function obj = clearPort(obj)
            a = instrfind;
            for i = 1:numel(a)
                if contains(string(a(i).name),string(obj.portName))
                    fclose(a(i));
                    delete(a(i));
                end
            end
        end
    end
end