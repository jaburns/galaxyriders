using UnityEngine;
using UnityEngine.SceneManagement;

public class PlayerController : MonoBehaviour
{
    struct InputFrame {
        public float AxisX;
        public bool JumpKey;
        public bool StompKey;
    };

    public float WalkForce;
    public float PumpForce;
    public float MaxRunSpeed;
    public float MaxPumpSpeed; // Instead of cutting off the pump, decay it at higher vels
    public float TurnAroundMultiplier;
    public float JumpSpeed;
    public int JumpLateFrames;

    public Color StompColor;
    public Color FlightColor;

    PlayerAnimator _animator;
    Rigidbody2D _rb;
    MeshRenderer _renderer;
    Color _originalColor;
    bool _wasDown;
    bool _jumpWasDown;

    ContactPoint2D[] _contacts;
    int _jumpLateFrames;
    bool _hasLandedFlat;

    public bool IsGrounded { get { return _jumpLateFrames > 0; } }
    public bool Touching { get { return _contacts != null; } }

    void Awake()
    {
        _rb = GetComponent<Rigidbody2D>();
        _renderer = GetComponent<MeshRenderer>();
        _originalColor = _renderer.material.color;
        _animator = GetComponentInChildren<PlayerAnimator>();
    }

    bool left { get { return Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow); } }
    bool right { get { return Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow); } }
    bool up { get { return Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow); } }
    bool down { get { return Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow); } }
    bool nuke { get { return Input.GetKey(KeyCode.N); } }
    bool nuked = false;

//     static string serializeFrames(IList<InputFrame> frames)
//     {
//         string ret = "";
//         for (int i = 0; i < frames.Count; ++i) {
//             ret += frames[i].AxisX + "`" + (frames[i].JumpKey?"1":"0") + "`" + (frames[i].StompKey?"1":"0") + "~";
//         }
//         return ret.Substring(0, ret.Length - 1);
//     }
//
//     static List<InputFrame> deserializeFrames(string frames)
//     {
//         var frameSet = frames.Split('~');
//         var ret = new List<InputFrame>();
//         for (int i = 0; i < frameSet.Length; ++i) {
//             var splits = frameSet[i].Split('`');
//             ret.Add(new InputFrame {
//                 AxisX = float.Parse(splits[0]),
//                 JumpKey = splits[1] == "1",
//                 StompKey = splits[2] == "1"
//             });
//         }
//         return ret;
//     }

    void FixedUpdate()
    {
        var axis = right ? 1 : left ? -1 : Input.GetAxis("Horizontal");
        var jumpKey = up || Input.GetButton("Jump");
        var stompKey = down || Input.GetButton("Grab");

        if (axis < -0.1f) transform.localScale = transform.localScale.WithX(-Mathf.Abs(transform.localScale.x));
        if (axis >  0.1f) transform.localScale = transform.localScale.WithX( Mathf.Abs(transform.localScale.x));

        var justJumped = jumpKey && !_jumpWasDown;

        var walkForce = axis * WalkForce * Vector2.right;
        if (_rb.velocity.x * axis < -1e-9) {
            walkForce *= TurnAroundMultiplier;
        } else {
            if (Mathf.Abs(_rb.velocity.x) > MaxRunSpeed) walkForce = Vector2.zero;
        }
        _rb.AddForce(walkForce, ForceMode2D.Force);

        if (nuke) {
            if (!nuked) {
                FindObjectOfType<DestructableLevel>().Nuke(transform.position.AsVector2());
                nuked = true;
            }
        } else {
            nuked = false;
        }

        if (stompKey) {
            if (!_wasDown) {
                _animator.Stomp();
                if (_rb.velocity.y > 0) _rb.velocity = _rb.velocity.WithY(0);
                _jumpLateFrames = 0;
            }
            if (_rb.velocity.y > -MaxPumpSpeed) {
                _rb.AddForce(-PumpForce * Vector2.up, ForceMode2D.Force);
            }
            _renderer.material.color = StompColor;
        } else {
            _renderer.material.color = !IsGrounded ? FlightColor : _originalColor;
        }

        if (_contacts != null) {
            foreach (var con in _contacts) {
                if (con.normal.y > .5f) {
                    _hasLandedFlat = true;
                    _animator.Land(con.normal);
                    break;
                }
            }
            if (_hasLandedFlat) {
                _jumpLateFrames = JumpLateFrames;
            }
        } else {
            if (_jumpLateFrames > 0) {
                _jumpLateFrames--;
            }
            if (_jumpLateFrames == 0) {
                _hasLandedFlat = false;
            }
        }

        if (IsGrounded && justJumped) {
            _jumpLateFrames = 0;
            _hasLandedFlat = false;
            _animator.Ollie();
            _rb.velocity += Vector2.up * JumpSpeed;
        }

        // There's an issue with Unity where it won't return multiple contact points on the same body, so wall collisions override ground sometimes.

        _jumpWasDown = jumpKey;
        _wasDown = stompKey;
        _contacts = null;
    }

    public void Bounce()
    {
        _rb.velocity = _rb.velocity.WithY(Mathf.Abs(_rb.velocity.y));
    }

    void OnCollisionEnter2D(Collision2D c)
    {
        OnCollisionStay2D(c);
    }

    void OnCollisionStay2D(Collision2D c)
    {
        _contacts = c.contacts;
    }

    void Die()
    {
        Debug.Log(Time.timeSinceLevelLoad);
        SceneManager.LoadScene(0);
    }
}
