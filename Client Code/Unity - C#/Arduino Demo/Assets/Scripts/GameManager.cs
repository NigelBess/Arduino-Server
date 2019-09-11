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
    private void Awake()
    {
        cm.Welcome();
        welcomeErrorText.text = "";
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
        SceneManager.LoadScene("Play");
    }
}
