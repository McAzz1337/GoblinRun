using UnityEngine;

public class GoblinChase : MonoBehaviour
{
    [Header("Target / Player")]
    [SerializeField] private Transform target; // vom Spawner gesetzt

    [Header("Movement")]
    [SerializeField] private float chaseSpeed = 1.4f;
    [SerializeField] private float wanderSpeed = 0.5f;
    [SerializeField] private float stopDistance = 0.8f;
    [SerializeField] private float groundHeightOffset = 0f;

    [Header("Behavior Distances")]
    [SerializeField] private float chaseRange = 3.0f;
    [SerializeField] private float giveUpRange = 5.0f;

    [Header("Wander Settings")]
    [SerializeField] private float wanderRadius = 2.0f;
    [SerializeField] private float wanderDecisionTime = 2.0f;

    [Header("Attack Hop Settings")]
    [SerializeField] private float attackHopHeight = 0.25f;
    [SerializeField] private float attackHopDuration = 0.2f;

    [Header("Hit Knockback")]
    [SerializeField] private float hitKnockbackDistance = 0.3f;
    [SerializeField] private float hitKnockbackDuration = 0.1f;

    private Vector3 _spawnPoint;
    private Vector3 _wanderTarget;
    private float _wanderTimer;
    private GoblinHealth _health;

    private float _attackHopTimer;

    private Vector3 _hitKnockbackDir;
    private float _hitKnockbackTimer;

    private enum State { Wander, Chase }
    private State _state = State.Wander;

    // NEU: erzwingt sofortiges Chase
    private bool _forceChase = false;

    void Start()
    {
        _spawnPoint = transform.position;
        PickNewWanderTarget();

        _health = GetComponent<GoblinHealth>();
    }

    public void SetTarget(Transform t)
    {
        target = t;
    }

    // NEU: wird vom Spawner bei Herald-Wave genutzt
    public void ForceChase()
    {
        _forceChase = true;
        _state = State.Chase;
    }

    void Update()
    {
        if (target == null) return;

        if (_health != null && _health.IsStaggered)
        {
            if (_hitKnockbackTimer > 0f)
            {
                DoHitKnockback();
            }

            ApplyHopAndGround();
            return;
        }

        if (_hitKnockbackTimer > 0f)
        {
            DoHitKnockback();
        }
        else
        {
            float distToPlayer = HorizontalDistance(transform.position, target.position);

            // NEU: wenn ForceChase aktiv ist, immer Chase
            if (_forceChase)
            {
                _state = State.Chase;
            }

            switch (_state)
            {
                case State.Wander:
                    if (!_forceChase && distToPlayer < chaseRange)
                    {
                        _state = State.Chase;
                    }
                    else if (!_forceChase)
                    {
                        DoWander();
                    }
                    else
                    {
                        DoChase();
                    }
                    break;

                case State.Chase:
                    // Gibt nur auf, wenn kein ForceChase aktiv ist
                    if (!_forceChase && distToPlayer > giveUpRange)
                    {
                        _state = State.Wander;
                        PickNewWanderTarget();
                    }
                    else
                    {
                        DoChase();
                    }
                    break;
            }
        }

        ApplyHopAndGround();
    }

    void DoChase()
    {
        Vector3 myPos = transform.position;
        Vector3 targetPos = target.position;
        myPos.y = 0f;
        targetPos.y = 0f;

        Vector3 toTarget = targetPos - myPos;
        float dist = toTarget.magnitude;

        if (dist > stopDistance)
        {
            Vector3 step = toTarget.normalized * chaseSpeed * Time.deltaTime;
            myPos += step;
        }

        if (toTarget.sqrMagnitude > 0.0001f)
        {
            transform.rotation = Quaternion.LookRotation(toTarget.normalized, Vector3.up);
        }

        myPos.y = groundHeightOffset;
        transform.position = myPos;
    }

    void DoWander()
    {
        _wanderTimer -= Time.deltaTime;
        if (_wanderTimer <= 0f)
        {
            PickNewWanderTarget();
        }

        Vector3 myPos = transform.position;
        Vector3 flatPos = myPos;
        flatPos.y = 0f;

        Vector3 flatTarget = _wanderTarget;
        flatTarget.y = 0f;

        Vector3 toTarget = flatTarget - flatPos;
        float dist = toTarget.magnitude;

        if (dist < 0.1f)
        {
            PickNewWanderTarget();
            return;
        }

        Vector3 step = toTarget.normalized * wanderSpeed * Time.deltaTime;
        flatPos += step;

        if (toTarget.sqrMagnitude > 0.0001f)
        {
            transform.rotation = Quaternion.LookRotation(toTarget.normalized, Vector3.up);
        }

        flatPos.y = groundHeightOffset;
        transform.position = flatPos;
    }

    void DoHitKnockback()
    {
        _hitKnockbackTimer -= Time.deltaTime;
        if (_hitKnockbackTimer <= 0f) return;

        float speed = hitKnockbackDistance / hitKnockbackDuration;

        Vector3 pos = transform.position;
        Vector3 flatPos = pos;
        flatPos.y = 0f;

        flatPos += -_hitKnockbackDir * speed * Time.deltaTime;

        flatPos.y = groundHeightOffset;
        transform.position = flatPos;
    }

    void ApplyHopAndGround()
    {
        float extraY = 0f;
        if (_attackHopTimer > 0f)
        {
            _attackHopTimer -= Time.deltaTime;
            float t = 1f - Mathf.Clamp01(_attackHopTimer / attackHopDuration);
            extraY = Mathf.Sin(t * Mathf.PI) * attackHopHeight;
        }

        Vector3 pos = transform.position;
        pos.y = groundHeightOffset + extraY;
        transform.position = pos;
    }

    void PickNewWanderTarget()
    {
        Vector2 circle = Random.insideUnitCircle * wanderRadius;
        Vector3 candidate = _spawnPoint + new Vector3(circle.x, 0f, circle.y);

        _wanderTarget = candidate;
        _wanderTimer = wanderDecisionTime;
    }

    float HorizontalDistance(Vector3 a, Vector3 b)
    {
        a.y = 0f;
        b.y = 0f;
        return Vector3.Distance(a, b);
    }

    public void PlayAttackHop()
    {
        _attackHopTimer = attackHopDuration;
    }

    public void ApplyHitKnockback(Vector3 hitDir)
    {
        hitDir.y = 0f;
        if (hitDir.sqrMagnitude < 0.0001f) return;

        _hitKnockbackDir = hitDir.normalized;
        _hitKnockbackTimer = hitKnockbackDuration;
    }
}
