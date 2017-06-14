using UnityEngine;
using System.Collections;

public class PlayerAnimator : MonoBehaviour
{
    [SerializeField] Sprite[] _staticFrames;
    [SerializeField] Sprite[] _ollieFrames;
    [SerializeField] Sprite[] _flipFrames;
    [SerializeField] Sprite[] _stompFrames;
    [SerializeField] Sprite[] _stompStaticFrames;

    SpriteRenderer _renderer;

    Sprite[] _thisAnim;
    int _frameIndex;

    bool _wasStomping;
    bool _wasGrounded;
    int _canLandcounter;

    void Awake()
    {
        _renderer = GetComponentInChildren<SpriteRenderer>();
        _renderer.sprite = _staticFrames[0];
        _thisAnim = _staticFrames;
        _frameIndex = 0;

        WorldPaletteLoader.Instance.OnChange.Sub(gameObject, () => {
            _renderer.color = WorldPaletteLoader.Instance.Palette.LifeColor;
        });
    }

    public void Ollie()
    {
        _thisAnim = _ollieFrames;
        _frameIndex = 0;
        _canLandcounter = 0;
    }

    public void Land(Vector2 norm)
    {
        if (_canLandcounter > 3) {
            _thisAnim = _staticFrames;
            _frameIndex = 0;
            transform.localRotation = Quaternion.Euler(0, 0, Mathf.Atan2(norm.y, norm.x) * Mathf.Rad2Deg - 90);
        }
    }

    public void Stomp()
    {
        if (_thisAnim == _ollieFrames || _thisAnim == _flipFrames) {
            _thisAnim = _stompFrames;
            _frameIndex = 0;
        }
    }

    void FixedUpdate()
    {
        _canLandcounter++;
    }

    void Update()
    {
        _frameIndex++;

        if (_frameIndex >= _thisAnim.Length) {
            if (_thisAnim == _ollieFrames) {
                _thisAnim = _flipFrames;
            }
            if (_thisAnim == _stompFrames) {
                _thisAnim = _stompStaticFrames;
            }
            _frameIndex = 0;
        }

        _renderer.sprite = _thisAnim[_frameIndex];

        if (_thisAnim == _ollieFrames || _thisAnim == _flipFrames) {
            transform.localRotation = Quaternion.Slerp(transform.localRotation, Quaternion.identity, 0.05f);
        }
    }
}
