clearAll;
arduino = Arduino('COM6');
arduino.connect();
arduino.encoder(2,3);
while true
    disp(arduino.readEncoder(2));
    if GetKey('Ii')
        arduino.resetEncoder(2);
    end
end