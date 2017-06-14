using UnityEngine;

static public class GameObjectExt
{
    static public T EnsureComponent<T> (this GameObject go) where T : Component
    {
        var getComp = go.GetComponent<T>();
        if (getComp != null) return getComp;
        return go.AddComponent<T>();
    }

    static public T GetComponentI <T> (this GameObject go) where T : class
    {
        return go.GetComponent (typeof (T)) as T;
    }

    static public T[] GetComponentsI <T> (this GameObject go) where T : class
    {
        return go.GetComponents (typeof (T)) as T[];
    }

    static public T GetComponentInParents <T> (this GameObject go) where T : Component
    {
        var c = go.GetComponent <T> ();
        if (c != null) return c;
        if (go.transform.parent == null) return null;
        return go.transform.parent.gameObject.GetComponentInParents <T> ();
    }

    static public GameObject InstantiateChild (this GameObject parent, GameObject prototype, bool preserveScale = false)
    {
        var child = UnityEngine.Object.Instantiate (prototype) as GameObject;
        var rotCache = child.transform.rotation;
        var scaleCache = child.transform.localScale;
        child.transform.position = parent.transform.position;
        child.transform.parent = parent.transform;
        if (!preserveScale) child.transform.localScale = scaleCache;
        child.transform.localRotation = rotCache;
        return child;
    }
}
