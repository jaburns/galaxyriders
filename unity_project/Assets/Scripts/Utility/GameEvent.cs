using System;
using UnityEngine;
using System.Collections.Generic;

public class GameEvent
{
    struct Subscriber
    {
        public GameObject gameObject;
        public Action action;
        public bool fireWhenNotActive;

        public bool HasValues(GameObject gameObject, Action action)
        {
            return this.gameObject == gameObject && this.action == action;
        }
    }

    readonly List<Subscriber> _subs = new List<Subscriber>();

    public void Sub(GameObject gameObject, Action action, bool fireWhenNotActive = false)
    {
        for(int i = 0; i < _subs.Count; i++) {
            if (_subs[i].HasValues(gameObject, action)) {
                return;
            }
        }

        _subs.Add(new Subscriber {
            gameObject = gameObject,
            action = action,
            fireWhenNotActive = fireWhenNotActive
        });
    }

    public void Unsub(GameObject gameObject, Action action)
    {
        for(int i = 0; i < _subs.Count; i++) {
            if (_subs[i].HasValues(gameObject, action)) {
                _subs.RemoveAt(i);
            }
        }
    }

    public void Broadcast()
    {
        for(int i = _subs.Count - 1; i >= 0; i--) {
            if (_subs[i].gameObject == null) {
                _subs.RemoveAt(i);
            } else if (_subs[i].gameObject.activeInHierarchy || _subs[i].fireWhenNotActive) {
                _subs[i].action();
            }
        }
    }
}

public class GameEvent<T>
{
    struct Subscriber<U>
    {
        public GameObject gameObject;
        public Action<U> action;
        public bool fireWhenNotActive;

        public bool HasValues(GameObject gameObject, Action<U> action)
        {
            return this.gameObject == gameObject && this.action == action;
        }
    }

    readonly List<Subscriber<T>> _subs = new List<Subscriber<T>>();

    public void Sub(GameObject gameObject, Action<T> action, bool fireWhenNotActive = false)
    {
        for(int i = 0; i < _subs.Count; i++) {
            if (_subs[i].HasValues(gameObject, action)) {
                return;
            }
        }

        _subs.Add(new Subscriber<T> {
            gameObject = gameObject,
            action = action,
            fireWhenNotActive = fireWhenNotActive
        });
    }

    public void Unsub(GameObject gameObject, Action<T> action)
    {
        for(int i = 0; i < _subs.Count; i++) {
            if (_subs[i].HasValues(gameObject, action)) {
                _subs.RemoveAt(i);
            }
        }
    }

    public void Broadcast(T args)
    {
        for(int i = _subs.Count - 1; i >= 0; i--) {
            if (_subs[i].gameObject == null) {
                _subs.RemoveAt(i);
            } else if (_subs[i].gameObject.activeInHierarchy || _subs[i].fireWhenNotActive) {
                _subs[i].action(args);
            }
        }
    }
}
