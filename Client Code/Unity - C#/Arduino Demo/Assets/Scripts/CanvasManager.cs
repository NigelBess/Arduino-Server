using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CanvasManager : MonoBehaviour
{
    [SerializeField] private GameObject[] menus;

    public void Welcome()
    {
        GameFunctions.OpenMenu(menus,0);
    }
    public void HUD()
    {
        GameFunctions.OpenMenu(menus, 1);
    }

}
