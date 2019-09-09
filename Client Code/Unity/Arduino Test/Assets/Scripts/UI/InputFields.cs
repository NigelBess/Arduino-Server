using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InputFields : MonoBehaviour
{
    [SerializeField] private InputField[] inputFields;
    [SerializeField] private DataLogger dataLogger;
    [SerializeField] private string fileName;

    void Awake()
    {
        Load();
    }
    public void Load()
    {
        string[] fieldContents = dataLogger.ReadArray(fileName);
        for (int i = 0; i < inputFields.Length; i++)
        {
            if (i<fieldContents.Length) inputFields[i].text = fieldContents[i];
        }
    }
    public virtual void Save()
    {
        for (int i = 0; i < inputFields.Length; i++)
        {
            dataLogger.Save(fileName, inputFields[i].text, i > 0);
        }
    }
}
