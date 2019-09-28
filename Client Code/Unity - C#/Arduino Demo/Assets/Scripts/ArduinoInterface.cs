using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.Diagnostics;

public class ArduinoInterface : MonoBehaviour
{
    private static byte[] terminator = {254};
    private byte[] incomingMessageBuffer = new byte[512];
    private const byte errorByte = 253;
    private const int readTimeout = 500;//ms

    private SerialPort port;
    public void SetPort(string nPortName)
    {
        port = new SerialPort();
        port.PortName = nPortName;
    }
    public void Connect()
    {
        port.BaudRate = 250000;
        port.DataBits = 8;
        port.Open();
    }
    public void SendMessage(byte[] message)
    {
        for (int i = 0; i < message.Length; i++)
        {
            port.Write(message,0,message.Length);
        }
        port.Write(terminator,0,1);
    }
    public byte[] GetMessage()
    {
        int i = 0;
        Stopwatch timer = Stopwatch.StartNew();
        while (i < incomingMessageBuffer.Length)
        {
            while (port.BytesToRead < 1)//wait for next byte to become available
            {
                if (timer.ElapsedMilliseconds > readTimeout)
                {
                    throw new System.TimeoutException("Arduino has timed out or disconnected.");
                }
            }
            byte incoming = (byte)port.ReadByte();
            if (incoming == terminator[0])
            {
                break;
            }
            incomingMessageBuffer[i] = incoming;
            i++;
        }
        i--;
        if (incomingMessageBuffer[0] == errorByte)
        {
            string errorMsg = "";
            for (int j = 1; j < i; j++)
            {
                errorMsg += (char)incomingMessageBuffer[j];
            }
            throw new System.InvalidOperationException(errorMsg);
        }
        byte[] output = new byte[i];
        for (int j = 0; j < i; j++)
        {
            output[j] = incomingMessageBuffer[j];
        }
        return output;
    }
}
