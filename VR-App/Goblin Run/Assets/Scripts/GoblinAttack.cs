using UnityEngine;

public class GoblinAttack : MonoBehaviour
{
    public float damage = 10f;
    public float attackInterval = 1.5f;
    private float lastAttackTime = 0f;

    private GoblinChase chase;

    void Awake()
    {
        // ganz wichtig: im Parent suchen
        chase = GetComponentInParent<GoblinChase>();
    }

    private void OnTriggerStay(Collider other)
    {
        TryDamage(other);
    }

    void TryDamage(Collider other)
    {
        if (Time.time - lastAttackTime < attackInterval) return;

        PlayerHealth ph = other.GetComponentInParent<PlayerHealth>();
        if (ph != null)
        {
            ph.TakeDamage(damage);
            lastAttackTime = Time.time;

            if (chase != null)
            {
                chase.PlayAttackHop();
            }
            else
            {
                Debug.LogWarning("GoblinAttack hat kein GoblinChase im Parent gefunden");
            }
        }
    }
}
