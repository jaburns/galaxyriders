using UnityEngine;
using System.Collections;

public class DeathTrigger : MonoBehaviour {

    // Use this for initialization
    void Start () {

    }

    // Update is called once per frame
    void Update () {

    }

    void OnTriggerEnter2D(Collider2D c)
    {
        c.gameObject.SendMessage("Die", null, SendMessageOptions.DontRequireReceiver);
    }
}
