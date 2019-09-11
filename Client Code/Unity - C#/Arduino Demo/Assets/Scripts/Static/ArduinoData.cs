using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class ArduinoData
{
    public static string port;
    public static string leftServoPin;
    public static string rightServoPin;
    public static string servoPowerPin;
    public static string encoderInterruptPin;
    public static string encoderSecondaryPin;
    public static string irSensorPin;
    public static string solenoidPin;
    private static Dictionary<string, int> dict = new Dictionary<string, int>
    {
        { "D0",0},
        { "D1",1},
        { "D2",2},
        { "D3",3},
        { "D4",4},
        { "D5",5},
        { "D6",6},
        { "D7",7},
        { "D8",8},
        { "D9",9},
        { "D10",10},
        { "D11",11},
        { "D12",12},
        { "D13",13},
        { "A0",14},
        { "A1",15},
        { "A2",16},
        { "A3",17},
        { "A4",18},
        { "A5",19},
    };
    public static int Pin(string pinName)
    {
        return dict[pinName];
    }
}
