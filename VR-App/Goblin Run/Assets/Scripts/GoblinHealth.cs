using UnityEngine;
using System.Collections;

public class GoblinHealth : MonoBehaviour
{
    [Header("Health")]
    [SerializeField] private int maxHealth = 3;
    [SerializeField] private float autoDespawnDistance = 20f;
    private int currentHealth;

    [Header("Hit / Stagger")]
    [SerializeField] private float knockbackForce = 6f;
    [SerializeField] private float upwardForce = 2f;
    [SerializeField] private float staggerDuration = 1.0f;
    [SerializeField] private Color hitColor = Color.red;
    [SerializeField] private float hitColorDuration = 0.1f;

    private Transform playerTarget;
    private Rigidbody rb;
    private Vector3 _lastHitDir;
    [SerializeField] private float maxVel = 5f;
    [SerializeField] private float maxAngularVel = 10f;

    private Renderer[] renderers;
    private Color[] originalColors;

    private bool isStaggered = false;
    public bool IsStaggered => isStaggered;

    private GoblinChase chase;

    void Start()
    {
        currentHealth = maxHealth;
        rb = GetComponent<Rigidbody>();
        chase = GetComponent<GoblinChase>();

        renderers = GetComponentsInChildren<Renderer>();
        originalColors = new Color[renderers.Length];
        for (int i = 0; i < renderers.Length; i++)
        {
            if (renderers[i] != null)
            {
                originalColors[i] = renderers[i].material.color;
            }
        }
    }

    void Update()
    {
        // Auto Despawn wenn zu weit weg
        if (playerTarget != null)
        {
            float dist = Vector3.Distance(transform.position, playerTarget.position);
            if (dist > autoDespawnDistance)
            {
                Destroy(gameObject);
            }
        }
    }

    public void SetPlayerTarget(Transform t)
    {
        playerTarget = t;
    }

    public void TakeHit(int damage, Vector3 hitPoint, Vector3 hitDirection)
    {
        _lastHitDir = hitDirection.normalized;
        currentHealth -= damage;

        if (chase != null)
        {
            chase.ApplyHitKnockback(hitDirection);
        }

        StartCoroutine(HitFeedbackRoutine());

        if (currentHealth <= 0)
        {
            Die();
        }
    }

    public void TakeHit(int damage)
    {
        TakeHit(damage, transform.position, Vector3.back);
    }

    IEnumerator HitFeedbackRoutine()
    {
        isStaggered = true;

        for (int i = 0; i < renderers.Length; i++)
        {
            if (renderers[i] != null)
            {
                renderers[i].material.color = hitColor;
            }
        }

        yield return new WaitForSeconds(hitColorDuration);

        for (int i = 0; i < renderers.Length; i++)
        {
            if (renderers[i] != null)
            {
                renderers[i].material.color = originalColors[i];
            }
        }

        float remaining = Mathf.Max(0f, staggerDuration - hitColorDuration);
        if (remaining > 0f)
        {
            yield return new WaitForSeconds(remaining);
        }

        isStaggered = false;
    }

    void Die()
    {
        GoblinChase chase = GetComponent<GoblinChase>();
        if (chase != null)
            chase.enabled = false;

        if (rb != null)
        {
            rb.isKinematic = false;
            rb.useGravity = true;
            rb.constraints = RigidbodyConstraints.None;

            // benutze jetzt knockbackForce und upwardForce
            Vector3 baseDir = (_lastHitDir + Vector3.up * upwardForce).normalized;
            rb.AddForce(baseDir * knockbackForce, ForceMode.Impulse);

            Vector3 randomDir = Random.onUnitSphere;
            randomDir.y = Mathf.Abs(randomDir.y) + 0.3f;
            rb.AddForce(randomDir * knockbackForce, ForceMode.Impulse);

            Vector3 v = rb.linearVelocity;
            if (v.magnitude > maxVel)
                rb.linearVelocity = v.normalized * maxVel;

            Vector3 av = rb.angularVelocity;
            if (av.magnitude > maxAngularVel)
            {
                rb.angularVelocity = av.normalized * maxAngularVel;
            }
        }

        Renderer rend = GetComponentInChildren<Renderer>();
        if (rend != null)
        {
            rend.material.color = Color.gray;
        }

        Destroy(gameObject, 4f);
    }
}
