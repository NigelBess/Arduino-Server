using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class GameFunctions : MonoBehaviour
{
    public static void OpenMenu(GameObject[] menus, int num)
    {
        for (int i = 0; i < menus.Length; i++)
        {
            if (menus[i] != null) menus[i].SetActive(i == num);

        }
    }
    public static void OpenMenu(List<GameObject> menus, int num)
    {
        for (int i = 0; i < menus.Count(); i++)
        {
            if (menus[i] != null) menus[i].SetActive(i == num);
        }
    }
}
