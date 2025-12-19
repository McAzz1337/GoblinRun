using UnityEngine;

public class WeatherController : MonoBehaviour
{
    [Header("References")]
    public GameObject rainFX;
    public AudioSource rainAudio;
    public Light mainLight;

    [Header("Fog Settings")]
    public FogMode fogMode = FogMode.ExponentialSquared;

    public Color clearFogColor = new Color(0.7f, 0.8f, 0.9f);
    public float clearFogDensity = 0.0f;
    public float clearFogStart = 0f;     // nur für Linear relevant
    public float clearFogEnd = 300f;     // nur für Linear relevant

    public Color rainFogColor = new Color(0.45f, 0.45f, 0.5f);
    public float rainFogDensity = 0.02f;
    public float rainFogStart = 0f;      // nur für Linear relevant
    public float rainFogEnd = 80f;       // nur für Linear relevant

    [Header("Light Settings")]
    public float clearLightIntensity = 1.2f;
    public float rainLightIntensity = 0.6f;

    private bool isRaining;

    public void SetMode(string mode)
    {
        if (string.IsNullOrEmpty(mode)) return;

        mode = mode.ToUpperInvariant();
        if (mode == "CLEAR") SetClear();
        else if (mode == "RAIN") SetRain();
        else Debug.LogWarning("WeatherController unbekannter Mode " + mode);
    }

    void SetClear()
    {
        isRaining = false;

        if (rainFX != null) rainFX.SetActive(false);
        if (rainAudio != null && rainAudio.isPlaying) rainAudio.Stop();

        ApplyFog(clearFogColor, clearFogDensity, clearFogStart, clearFogEnd);

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

        ApplyFog(rainFogColor, rainFogDensity, rainFogStart, rainFogEnd);

        if (mainLight != null)
        {
            mainLight.intensity = rainLightIntensity;
            mainLight.color = new Color(0.8f, 0.85f, 0.9f);
        }

        Debug.Log("Weather RAIN aktiviert");
    }

    void ApplyFog(Color color, float density, float start, float end)
    {
        RenderSettings.fog = density > 0f || fogMode == FogMode.Linear; // Linear kann auch mit 0 density aktiv sein
        RenderSettings.fogMode = fogMode;
        RenderSettings.fogColor = color;

        if (fogMode == FogMode.Linear)
        {
            RenderSettings.fogStartDistance = start;
            RenderSettings.fogEndDistance = Mathf.Max(start + 0.01f, end);
        }
        else
        {
            RenderSettings.fogDensity = density;
        }
    }
}
