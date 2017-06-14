using System;
using UnityEngine;

public class CallbackTrigger : MonoBehaviour
{
    GameEvent<Collider2D> _enter;
    GameEvent<Collider2D> _exit;

    public void Init(GameObject gameObject, Action<Collider2D> onEnter, Action<Collider2D> onExit)
    {
        if (_enter == null && onEnter != null) _enter = new GameEvent<Collider2D>();
        if (_exit == null && onExit != null) _exit = new GameEvent<Collider2D>();

        if (onEnter != null) _enter.Sub(gameObject, onEnter);
        if (onExit != null) _exit.Sub(gameObject, onExit);
    }

    void OnTriggerEnter2D(Collider2D col)
    {
        if (_enter != null) _enter.Broadcast(col);
    }

    void OnTriggerExit2D(Collider2D col)
    {
        if (_exit != null) _exit.Broadcast(col);
    }
}
