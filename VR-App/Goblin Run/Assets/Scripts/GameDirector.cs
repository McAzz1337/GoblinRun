using UnityEngine;

public class GameDirector : MonoBehaviour
{
    public GoblinSpawner goblinSpawner;
    public WeatherController weatherController;
    public ScreenBlocker screenBlocker;
    public TrapManager trapManager;

    public void Execute(CommandMessage cmd)
    {
        if (cmd == null || string.IsNullOrEmpty(cmd.type))
        {
            Debug.LogWarning("GameDirector.Execute: leerer Command");
            return;
        }

        var type = cmd.type.ToUpperInvariant();

        switch (type)
        {
            case "SPAWN_ENEMY":
                HandleSpawnEnemy(cmd);
                break;

            case "SET_WEATHER":
                HandleSetWeather(cmd);
                break;

            case "SCREEN_BLOCK":
                HandleScreenBlock(cmd);
                break;

            case "DEBUG_MESSAGE":
                Debug.Log("DEBUG_MESSAGE von Controller " + cmd.text);
                break;

            case "TRAP":
            case "TRIGGER_TRAP":
                HandleTrap(cmd);
                break;

            default:
                Debug.LogWarning("GameDirector: unbekannter Command Typ " + type);
                break;
        }
    }

    void HandleSpawnEnemy(CommandMessage cmd)
    {
        if (goblinSpawner == null)
        {
            Debug.LogWarning("GameDirector: GoblinSpawner nicht gesetzt");
            return;
        }

        goblinSpawner.SpawnFromCommand(cmd.enemyType, cmd.position);
    }

    void HandleSetWeather(CommandMessage cmd)
    {
        if (weatherController == null)
        {
            Debug.LogWarning("GameDirector: WeatherController nicht gesetzt");
            return;
        }

        weatherController.SetMode(cmd.mode);
    }

    void HandleScreenBlock(CommandMessage cmd)
    {
        if (screenBlocker == null)
        {
            Debug.LogWarning("GameDirector: ScreenBlocker nicht gesetzt");
            return;
        }

        var mode = cmd.mode != null ? cmd.mode.ToUpperInvariant() : "";
        bool blocked = mode == "ON";
        screenBlocker.SetBlocked(blocked);
    }

    void HandleTrap(CommandMessage cmd)
    {
        if (trapManager == null)
        {
            Debug.LogWarning("GameDirector: TrapManager nicht gesetzt");
            return;
        }

        if (string.IsNullOrEmpty(cmd.trapId))
        {
            Debug.LogWarning("GameDirector: trapId fehlt im Command");
            return;
        }

        trapManager.TriggerTrap(cmd.trapId);
    }
}
