using UnityEngine;

public class Trap : MonoBehaviour
{
    public string trapId;

    [Header("Optional")]
    public Animator animator;
    public string triggerName = "Activate";

    public AudioSource audioSource;
    public Collider hurtCollider;

    public void Activate()
    {
        Debug.Log($"Trap {trapId} aktiviert");

        if (animator != null && !string.IsNullOrEmpty(triggerName))
        {
            animator.SetTrigger(triggerName);
        }

        if (audioSource != null)
        {
            audioSource.Play();
        }

        if (hurtCollider != null)
        {
            StartCoroutine(EnableColliderShort());
        }
    }

    System.Collections.IEnumerator EnableColliderShort()
    {
        hurtCollider.enabled = true;
        yield return new WaitForSeconds(0.5f);
        hurtCollider.enabled = false;
    }
}
