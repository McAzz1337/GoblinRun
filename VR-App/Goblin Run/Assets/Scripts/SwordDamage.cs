using UnityEngine;

public class SwordDamage : MonoBehaviour
{
    [SerializeField] private int damagePerHit = 1;
    [SerializeField] private float hitCooldown = 0.2f;
    private float lastHitTime = 0f;

    private void OnTriggerEnter(Collider other)
    {
        if (Time.time - lastHitTime < hitCooldown) return;

        GoblinHealth gh = other.GetComponentInParent<GoblinHealth>();
        if (gh != null)
        {
            Vector3 hitPoint = other.ClosestPoint(transform.position);
            Vector3 hitDir = transform.forward;

            gh.TakeHit(damagePerHit, hitPoint, hitDir);
            lastHitTime = Time.time;
        }
    }
}
