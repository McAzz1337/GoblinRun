using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class PlayerHealth : MonoBehaviour
{
    [Header("Health Settings")]
    [SerializeField] private float maxHealth = 100f;
    [SerializeField] private float regenRate = 5f;          // wieviel HP pro Sekunde
    [SerializeField] private float regenDelay = 3f;         // wie lange warten nach Schaden
    private float currentHealth;
    private float lastDamageTime;

    [Header("UI")]
    [SerializeField] private Image healthFill;              // dein Healthbar_Fill
    [SerializeField] private TMP_Text healthText;           // dein Healthbar_Text

    void Start()
    {
        currentHealth = maxHealth;
        UpdateUI();
    }

    void Update()
    {
        // Regeneration nach einer Weile
        if (currentHealth < maxHealth)
        {
            // genug Zeit seit letztem Schaden vergangen?
            if (Time.time - lastDamageTime >= regenDelay)
            {
                currentHealth += regenRate * Time.deltaTime;
                currentHealth = Mathf.Min(currentHealth, maxHealth);
                UpdateUI();
            }
        }
    }

    public void TakeDamage(float amount)
    {
        currentHealth -= amount;
        currentHealth = Mathf.Max(currentHealth, 0f);
        lastDamageTime = Time.time;
        UpdateUI();

        if (currentHealth <= 0f)
        {
            OnDeath();
        }
    }

    private void OnDeath()
    {
        Debug.Log("Player died (placeholder).");
        // TODO: Respawn / Game Over / Fade / Controller benachrichtigen
    }

    private void UpdateUI()
    {
        if (healthFill != null)
        {
            healthFill.fillAmount = currentHealth / maxHealth;
        }

        if (healthText != null)
        {
            // Variante 1: "75 / 100"
            healthText.text = Mathf.RoundToInt(currentHealth) + " / " + Mathf.RoundToInt(maxHealth);

            // Variante 2: "75%"
            // float pct = (currentHealth / maxHealth) * 100f;
            // healthText.text = Mathf.RoundToInt(pct) + " %";
        }
    }

    // falls später Goblins über Collider Schaden machen wollen:
    public void TakeDamage(int amount)
    {
        TakeDamage((float)amount);
    }

    public float CurrentHealth => currentHealth;
    public float MaxHealth => maxHealth;
}
