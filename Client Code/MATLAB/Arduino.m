classdef Arduino < handle
    properties(Constant)
        baudRate = 250000;
        nullByte = 255;
        terminator = 254;
        errorByte = 253;
        messageBufferSize = 512;
        connectionTimeOut = 2;
        serialTimeOut = 1;
        maxAnalogRead = 5;%volts
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
            obj.messageBuffer = zeros(1,obj.messageBufferSize);
            obj.setupDictionary;
        end
        function obj = connect(obj)
            obj.clearPort();
            obj.comPort = serial(obj.portName);
            obj.comPort.BaudRate = obj.baudRate;
            fopen(obj.comPort);
            flushoutput(obj.comPort);
            flushinput(obj.comPort);
            obj.waitForConnection();
            disp("Connected to " + obj.getDeviceInfo());  
        end
        function pinMode(obj,pin,type)
           pin = obj.int8(pin);
           type = obj.int8(type);
            reply = obj.sendMessageReliable([0,pin,type]);
            if reply == obj.errorByte
                error(obj.multiError("Pin setup", obj.pinError(pin), obj.typeError(type,"pin type")),0);
            end
        end
        
        function digitalWrite(obj,pin,state)
            pin = obj.int8(pin);
            state = obj.int8(state);
            reply = obj.sendMessageReliable([1,pin,state]);
            if reply == obj.errorByte
                error(obj.multiError("Writing digital pin",obj.pinError(pin),obj.typeError(state,"digital pin state")),0)
            end
        end
        function analogWrite(obj,pin,state)
            pin = obj.int8(pin);
            state = obj.int8(state);
            reply = obj.sendMessageReliable([2,pin,state]);
            if reply == obj.errorByte
                error(obj.multiError("Writing analog pin",obj.pinError(pin),obj.typeError(state,"value between 0 and 252")),0);
            end
        end
        function out = digitalRead(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([3,pin]);
            if reply == obj.errorByte
                error(obj.multiError("Reading digital pin",obj.pinError(pin)),0);
            end
            out = logical(reply);
        end
        function out = analogRead(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([4,pin]);
            if reply == obj.errorByte
                error(obj.multiError("Reading digital pin",obj.pinError(pin)),0);
            end
            out = obj.parseInt(reply)*obj.maxAnalogRead/1023;
        end
        function attachServo(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([5,pin]);
            if reply == obj.errorByte
                error(obj.multiError("Attaching servo",obj.pinError(pin)),0);
            end
        end
        function writeServo(obj,pin,angle)
            pin = obj.int8(pin);
            angle = obj.int8(angle);
            reply = obj.sendMessageReliable([6,pin,angle]);
            if reply == obj.errorByte
                error(obj.multiError("Writing position to servo",obj.pinError(pin),obj.typeError(angle,"angle between 0 and 180 degrees")),0);
            end
        end
        function detachServo(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([7,pin]);
            if reply == obj.errorByte
                error("No servo found at pin " + string(pin));
            end
        end
        function encoder(obj,interruptPin,secondaryPin)
            if nargin<3
                secondaryPin = obj.nullByte;
            end
            interruptPin = obj.int8(interruptPin);
            secondaryPin = obj.int8(secondaryPin);
            reply = obj.sendMessageReliable([8,interruptPin,secondaryPin]);
            if reply == obj.errorByte
                error(obj.multiError("Setting up encoder",obj.pinError(interruptPin))+"\n OR\n Pin " + string(interruptPin) + " is not an interrupt pin on this Arduino.",0);
            end
        end
        function out = getEncoderCount(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([9,pin]);
            disp(reply);
            if reply == obj.errorByte
                error("No encoder found at pin " + string(pin));
            end
            out = obj.parseInt(reply);
        end
        function resetEncoder(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([10,pin]);
            if reply == obj.errorByte
                error("No encoder found at pin " + string(pin));
            end
        end
        function detachEncoder(obj,pin)
            pin = obj.int8(pin);
            reply = obj.sendMessageReliable([11,pin]);
            if reply == obj.errorByte
                error("No encoder found at pin " + string(pin));
            end
        end
        function setEncoderDirection(obj,pin,direction)
            pin = obj.int8(pin);
            direction = obj.int8(direction);
            reply = obj.sendMessageReliable([12,pin,direction]);
            if reply == obj.errorByte
                error("No encoder found at pin " + string(pin) + "\nOR\n" + string(direction)+" is invalid. Valid values are 0 (no count) 1 (positive count) and 2 (negative count).");
            end
        end
        
        
        function waitForConnection(obj)  
            numReads = 10;%successful messages sent before we determine that we are definitely connected
            numSlowReads = 2;%first this many reads will be given some extra time
            for i = 1:numReads
                connected = obj.checkConnection(i<=numSlowReads);
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
            try
                flushoutput(obj.comPort);
            catch
                error("Arduino is not connected. Run Arduino.connect() to connect.");
            end
            fwrite(obj.comPort,[msg,obj.terminator]);
        end
        function out = sendMessageReliable(obj,msg)
            obj.sendMessage(msg);
            out = obj.getMessage();
        end
        function out = getMessage(obj,timeOutTime)
            obj.messageBuffer = zeros(1,numel(obj.messageBuffer));
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
                    out = obj.messageBuffer(1:index-1);
                    return;
                end
                obj.messageBuffer(index) = newByte;
            end
            out = obj.messageBuffer(1:index);
        end
        function out = parseInt(obj,msg)
            sign = 1;
            if (msg(1)) sign = -1; end
            out = 0;
            for i = 2:numel(msg)
                out = out + msg(i)*2^(7*(i-2));
            end
            out = out*sign;
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
        function in = int8(obj,in)
            if islogical(in)
                in = double(in);
            end
            if ~isnumeric(in)
                try
                in = obj.dictionary(in);
                catch
                    error("Invalid input parameter.");
                end
            end
            in = floor(in);
            if in<0
                error("Input should be positive");
            end
            in = min(in,255);
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
            out = string(value) + " is not a valid " + type +".";
        end
    end
end