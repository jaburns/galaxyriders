using UnityEngine;
using System.Collections;

public class Autodestruct : MonoBehaviour
{
    public float Delay;

    IEnumerator Start()
    {
        yield return new WaitForSeconds(Delay);
        Destroy(gameObject);
    }
}
