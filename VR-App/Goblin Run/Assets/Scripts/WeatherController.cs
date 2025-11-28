using UnityEngine;

public class WeatherController : MonoBehaviour
{
    [Header("References")]
    public GameObject rainFX;          // Particles für Regen
    public AudioSource rainAudio;      // Regen Sound optional
    public Light mainLight;            // deine Sonne im Rig

    [Header("Fog Settings")]
    public Color clearFogColor = new Color(0.7f, 0.8f, 0.9f);
    public float clearFogDensity = 0.0f;

    public Color rainFogColor = new Color(0.45f, 0.45f, 0.5f);
    public float rainFogDensity = 0.02f;

    [Header("Light Settings")]
    public float clearLightIntensity = 1.2f;
    public float rainLightIntensity = 0.6f;

    private bool isRaining = false;

    public void SetMode(string mode)
    {
        if (string.IsNullOrEmpty(mode))
            return;

        mode = mode.ToUpperInvariant();

        switch (mode)
        {
            case "CLEAR":
                SetClear();
                break;

            case "RAIN":
                SetRain();
                break;

            default:
                Debug.LogWarning("WeatherController unbekannter Mode " + mode);
                break;
        }
    }

    void SetClear()
    {
        isRaining = false;

        if (rainFX != null) rainFX.SetActive(false);
        if (rainAudio != null && rainAudio.isPlaying) rainAudio.Stop();

        RenderSettings.fog = clearFogDensity > 0f;
        RenderSettings.fogColor = clearFogColor;
        RenderSettings.fogDensity = clearFogDensity;

        if (mainLight != null)
        {
            mainLight.intensity = clearLightIntensity;
            mainLight.color = Color.white;
        }

        Debug.Log("Weather CLEAR aktiviert");
    }

    void SetRain()
    {
        isRaining = true;

        if (rainFX != null) rainFX.SetActive(true);
        if (rainAudio != null && !rainAudio.isPlaying) rainAudio.Play();

        RenderSettings.fog = true;
        RenderSettings.fogColor = rainFogColor;
        RenderSettings.fogDensity = rainFogDensity;

        if (mainLight != null)
        {
            mainLight.intensity = rainLightIntensity;
            mainLight.color = new Color(0.8f, 0.85f, 0.9f);
        }

        Debug.Log("Weather RAIN aktiviert");
    }
}
