classdef Arduino < handle
    properties(Constant)
        baudRate = 250000;
        terminator = 254;
        messageBufferSize = 512;
        connectionTimeOut = 2;
        serialTimeOut = 0.1;
    end
    properties(Access = private)
        comPort;
        messageBuffer;
        portName;
        dictionary;
    end
    
    methods(Access = public)
        function obj = Arduino(port)
            obj.portName = port;
            obj.clearPort();
            obj.messageBuffer = zeros(1,obj.messageBufferSize);
            obj.setupDictionary;
        end
        function obj = connect(obj)
            obj.comPort = serial(obj.portName);
            obj.comPort.BaudRate = obj.baudRate;
            fopen(obj.comPort);
            flushoutput(obj.comPort);
            flushinput(obj.comPort);
            obj.waitForConnection();
            disp("Connected to " + obj.getDeviceInfo());  
        end
        function setupPin(obj,pin,type)
           pin = obj.int(pin);
           type = obj.int(type);
            reply = obj.sendMessageReliable([0,pin,type]);
            if reply == 253
                error(obj.multiError("Pin setup", obj.pinError(pin), obj.typeError(type,"pin type")),0);
            end
        end
        
        function writeDigitalPin(obj,pin,state)
            pin = obj.int(pin);
            state = obj.int(state);
            reply = obj.sendMessageReliable([1,pin,state]);
            if reply == 253
                error(obj.multiError("Writing digital pin",obj.pinError(pin),obj.typeError(state,"digital pin state")),0)
            end
        end
        
        
        function waitForConnection(obj)  
            for i = 1:2
                connected = obj.checkConnection(true);
            end
            while ~connected
               	connected = obj.checkConnection();
            end
        end
        function out = checkConnection(obj,firstTime)
            if nargin<2
                firstTime = false;
            end
            obj.sendMessage(253);
            if ~firstTime
                msg = obj.getMessage();
            else
                msg = obj.getMessage(obj.connectionTimeOut);
            end
            out = prod(msg == 0);
        end
        function out = getDeviceInfo(obj)
            obj.sendMessage([253,0]);
            out = obj.getMessageAsText();
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
            flushoutput(obj.comPort);
            fwrite(obj.comPort,[msg,obj.terminator]);
        end
        function out = sendMessageReliable(obj,msg)
            obj.sendMessage(msg);
            out = obj.getMessage();
        end
        function out = getMessage(obj,timeOutTime)
            if nargin<2
                timeOutTime = obj.serialTimeOut;
            end
            index = 0;
            startTime = obj.getTime();
            while (index<=(obj.messageBufferSize-1))
                index = index+1;   
                while (~obj.comPort.BytesAvailable)
                    if((obj.getTime()-startTime)>timeOutTime)
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
        function obj = setupDictionary(obj)
            %stores string to int conversions for various function inputs
            d = containers.Map;
            
            %digital pins
            for i = 0:13
                d("D" + string(i)) = i;
            end
            %analog pins
            for i = 0:5
                d("A" + string(i)) = i+14;
            end
            
            %pin types
            d("INPUT") = 0;
            d("OUTPUT") = 1;
            d("INPUT_PULLUP") = 2;    
            
            %digital output states
            d("HIGH") = 1;
            d("LOW") = 0;
            
            obj.dictionary = d;
        end
        function in = int(obj,in)
            if ~isnumeric(in)
                try
                in = obj.dictionary(in);
                catch
                    error("Invalid input parameter.");
                end
            end
            in = floor(in);
        end
        function str = multiError(obj,attemptStr,varargin)
            str = attemptStr + " failed. Possible causes:\n";
            for i = 1:numel(varargin)
                str = str +"\n "+  varargin{i};
                if i<numel(varargin)
                    str = str + "\n OR";
                end
            end
        end
        function out = pinError(obj,pin)
            out = "Pin " + string(pin) + " is not valid or is in use by a servo or encoder.";
        end
        function out = typeError(obj,value,type)
            out = string(value) + " does not name a valid " + type +".";
        end
    end
end