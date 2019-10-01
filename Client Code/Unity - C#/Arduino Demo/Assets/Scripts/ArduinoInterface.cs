using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.Diagnostics;

public class ArduinoInterface : MonoBehaviour
{
    private static byte terminator = 254;
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
        port.NewLine = ((char)terminator).ToString();
        port.Encoding = System.Text.Encoding.UTF8;
        port.Open();
        port.DiscardOutBuffer();
        port.DiscardInBuffer();
        WaitForConnection();

    }
    public void WaitForConnection()
    {
        bool connected = CheckConnection();
        while (!connected)
        {
            connected = CheckConnection();
        }
    }
    public bool CheckConnection()
    {
        SendMessage(new byte[1] { 253 });
        byte[] response = GetMessage(false);
        return (response[0] == 0);
    }
    public void Disconnect()
    {
        port.Close();
    }
    public byte[] sendMessageReliable(byte[] message)
    {
        SendMessage(message);
        
        byte[] returnMsg =  GetMessage();
        return returnMsg;
        
    }
    public void SendMessage(byte[] message)
    {
        byte[] terminatedMessage = new byte[message.Length + 1];
        for (int i = 0; i < message.Length; i++)
        {
            terminatedMessage[i] = message[i];
        }
        terminatedMessage[terminatedMessage.Length - 1] = terminator;
        port.Write(terminatedMessage,0,terminatedMessage.Length);
        port.DiscardOutBuffer();
    }
    public byte[] GetMessage()
    {
        return GetMessage(true);
    }
    public byte[] GetMessage(bool safe)
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
            if (incoming == terminator)
            {
                break;
            }
            incomingMessageBuffer[i] = incoming;
            i++;
        }
        if (safe && incomingMessageBuffer[0] == errorByte)
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
        port.DiscardInBuffer();
        return output;
    }
}
