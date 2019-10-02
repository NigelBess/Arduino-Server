using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Servo : MonoBehaviour
{
    [SerializeField] private ArduinoInterface arduino;
    private int pin;
    bool pinSetup = false;
    public void SetPin(string newPin)
    {
        pin = ArduinoData.Pin(newPin);
    }
    public void SendAngle(float angle)
    {
        if (!pinSetup)
        {
            arduino.AttachServo(pin);
            pinSetup = true;
        }
        arduino.WriteServo(pin, Mathf.FloorToInt(angle));
    }
}
