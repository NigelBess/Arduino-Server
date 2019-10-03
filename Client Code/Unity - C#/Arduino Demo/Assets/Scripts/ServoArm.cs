using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ServoArm : MonoBehaviour
{
    [SerializeField] private Servo servo;
    private Transform parent;
    bool clicked;
    private float hitHeight;
    private const float refreshTime = 0.01f;
    private float lastSendTime = 0;
    private void Awake()
    {
        parent = transform.parent;
    }
    private void Update()
    {
        if (!clicked) return;
        if (!Input.GetMouseButton(0))
        {
            clicked = false;
            return;
        }
        Plane plane = new Plane(Vector3.up, -hitHeight);
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        float enter;
        plane.Raycast(ray,out enter);
        Vector3 hitPoint = ray.GetPoint(enter);
        Quaternion newRot = Quaternion.LookRotation(parent.position-hitPoint, Vector3.up);
        float angle = normalized(newRot.eulerAngles.y-parent.parent.rotation.eulerAngles.y);
        if (angle > 270) angle = 0;
        angle = Mathf.Clamp(angle, 0, 180);
        parent.localRotation = Quaternion.Euler(0,angle,0);
        Debug.DrawLine(parent.position,hitPoint);
        SendAngle(180-angle);

    }
    private void OnMouseDown()
    {
        clicked = true;
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;
        if (Physics.Raycast(ray, out hit))
        {
            hitHeight = hit.point.y;
        }
    }
    private float Angle(float x, float z)
    {
        return Mathf.Atan(x - parent.transform.position.x/ z-parent.transform.position.z) * 180 / Mathf.PI;
    }
    private float normalized(float angle)
    {
        Debug.Log(angle);
        angle %= 360;
        if (angle < 0)
        {
            angle += 360;
        }
        return angle;
    }
    private void SendAngle(float angle)
    {
        if ((Time.time - lastSendTime) < refreshTime) return;
        servo.SendAngle(angle);
        lastSendTime = Time.time;
        Debug.Log(angle);
    }
}

