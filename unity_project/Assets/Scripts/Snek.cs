using UnityEngine;

public class Snek : MonoBehaviour
{
    [SerializeField] Rigidbody2D[] Balls;

    Vector2 _origin;

    void Awake()
    {
        _origin = transform.position.AsVector2();

        for (int i = 0; i < Balls.Length; ++i) {
            Balls[i].gameObject.AddComponent<CallbackTrigger>().Init(gameObject, OnBallEnter, null);
        }
    }

    Vector2 getPosition(float t)
    {
        return _origin + new Vector2 {
            x = 10 * Mathf.Cos(  t),
            y =  5 * Mathf.Sin(2*t)
        };
    }

    void FixedUpdate()
    {
        for (int i = 0; i < Balls.Length; ++i) {
            Balls[i].MovePosition(getPosition(Time.time - i*0.1f));
        }
    }

    void OnBallEnter(Collider2D col)
    {
        var player = col.GetComponent<PlayerController>();
        if (player != null) {
            player.Bounce();
        }
    }
}
