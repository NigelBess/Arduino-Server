using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour
{
    [SerializeField] private ArduinoInterface arduino;
    [SerializeField] private CanvasManager cm;
    [SerializeField] private Text welcomeErrorText;
    [SerializeField] private InputField manualInputField;
    [SerializeField] private Text manualInputReturnText;
    private void Awake()
    {
        cm.Welcome();
        welcomeErrorText.text = "";
        Log("");
    }
    public void Begin()
    {
        arduino.SetPort(ArduinoData.port);
        try
        {
            arduino.Connect();
        } catch (System.Exception e)
        {
            welcomeErrorText.text = e.Message;
            return;
        }
        cm.HUD();
    }
    public void Reset()
    {
        arduino.Disconnect();
        SceneManager.LoadScene("Play");
    }
    public void SendMessageManual()
    {
        string msg = manualInputField.text;
        string[] values = msg.Split(',');
        if (values.Length < 1)
        {
            return;
        }
        byte[] arduinoMessage = new byte[values.Length];
        for (int i = 0; i < values.Length; i++)
        {
            try
            {
                arduinoMessage[i] = byte.Parse(values[i]);
            }
            catch
            {
                Log("Failed to parse message. Messages have the form: value1,value2,...valueN. Values must be between 0 and 255");
                return;
            }
        }
        DebugByteArray(arduinoMessage);
        byte[] returnMessage = new byte[0];
        try
        {
            returnMessage = arduino.sendMessageReliable(arduinoMessage);
            Log(returnMessage);
            Debug.Log("message send successful");
        }
        catch (System.InvalidOperationException e)
        {
            Log(e.Message); ;
            Debug.Log("message failed");
        }
    }
    public void Log(string msg)
    {
        manualInputReturnText.text = msg;
    }
    public void Log(byte[] msg)
    {
        string str = "";
        for (int i = 0; i < msg.Length; i++)
        {
            str += (msg[i]).ToString()+ " ";
        }
        Log(str);
    }
    public void DebugByteArray(byte[] msg)
    {

        string str = "";
        for (int i = 0; i < msg.Length; i++)
        {
            str += msg[i].ToString();
            if (i < msg.Length - 1)
            {
                str += ", ";
            }
        }
        Debug.Log(str);
    }
}
