using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ArduinoPortFields : InputFields
{ 
    [SerializeField] private InputField comPortField;
    [SerializeField] private InputField leftServoField;
    [SerializeField] private InputField rightServoField;
    [SerializeField] private InputField servoPowerField;
    [SerializeField] private InputField encoderInterruptField;
    [SerializeField] private InputField encoderSecondaryField;
    [SerializeField] private InputField iRSensorField;
    [SerializeField] private InputField solenoidField;
    [SerializeField] private Servo leftServo;

    private void Awake()
    {
        inputFields = new InputField[] { comPortField, leftServoField, rightServoField, servoPowerField, encoderInterruptField, encoderSecondaryField, iRSensorField, solenoidField};
        Load();
    }

    public override void Save()
    {
        ArduinoData.port = comPortField.text;
        ArduinoData.leftServoPin = leftServoField.text;
        ArduinoData.rightServoPin = rightServoField.text;
        ArduinoData.servoPowerPin = servoPowerField.text;
        ArduinoData.encoderInterruptPin = encoderInterruptField.text;
        ArduinoData.encoderSecondaryPin = encoderSecondaryField.text;
        ArduinoData.irSensorPin = iRSensorField.text;
        ArduinoData.solenoidPin = solenoidField.text;
        base.Save();
        leftServo.SetPin(ArduinoData.leftServoPin);
    }
}
