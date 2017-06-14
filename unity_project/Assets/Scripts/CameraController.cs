using UnityEngine;
using UnityStandardAssets.ImageEffects;

public class CameraController : MonoBehaviour
{
    public GameObject Target;
    public Vector2 OffsetRate;
    public Vector2 OffsetSize;

    Camera _camera;
    Rigidbody2D _targetRb;
    MotionBlur _motionBlur;
    Vector2 _offset;

    void Awake()
    {
        _camera = GetComponent<Camera>();
        _targetRb = Target.GetComponent<Rigidbody2D>();
        _motionBlur = GetComponent<MotionBlur>();

        WorldPaletteLoader.Instance.OnChange.Sub(gameObject, () => {
            _camera.backgroundColor = WorldPaletteLoader.Instance.Palette.SpaceColor;
        });
    }

    void LateUpdate()
    {
        if (_motionBlur.enabled) {
            var blur = (_targetRb.velocity.magnitude - 20) / 100;
            if (blur < 0) blur = 0;
            if (blur > 5) blur = 5;
            _motionBlur.blurAmount = blur;
        }

        _offset.x += _targetRb.velocity.x * OffsetRate.x;
        _offset.y += _targetRb.velocity.y * OffsetRate.y;
        if (_offset.x >  OffsetSize.x) _offset.x =  OffsetSize.x;
        if (_offset.x < -OffsetSize.x) _offset.x = -OffsetSize.x;
        if (_offset.y >  OffsetSize.y) _offset.y =  OffsetSize.y;
        if (_offset.y < -OffsetSize.y) _offset.y = -OffsetSize.y;

        transform.position = Target.transform.position.WithZ(transform.position.z) + _offset.AsVector3();
    }
}
