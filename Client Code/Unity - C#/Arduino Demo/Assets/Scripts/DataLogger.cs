using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class DataLogger : MonoBehaviour
{
    [SerializeField] private string directory;
    public void Save(string fileName, string data, bool append)
    {
        string path = Path();
        if (!Directory.Exists(path))
        {
            Directory.CreateDirectory(path);
        }
        path += "/" + fileName + ".csv";
        if (!File.Exists(path))
        {
            FileStream file = File.Create(path);
            file.Close();
        }
        using (TextWriter writer = new StreamWriter(path, append: append))
        {
            writer.WriteLine(data+",");
        }
    }
    public string Read(string fileName)
    {
        string path = Path();
        path += "/" + fileName + ".csv";
        if (!File.Exists(path))
        {
            throw new System.IO.FileNotFoundException("Attempting to read a file that does not exist.");
        }
        return File.ReadAllText(path);
    }
    public string[] ReadArray(string fileName)
    {
        string contents = Read(fileName);
        string[] separated = contents.Split(',');
        for (int i = 0; i < separated.Length; i++)
        {
            separated[i] = separated[i].Trim();
        }
        return separated;
    }
    private string Path()
    {
        return Application.persistentDataPath + "/" + directory;
    }
}