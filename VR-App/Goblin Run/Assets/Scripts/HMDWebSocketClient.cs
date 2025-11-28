using System;
using System.Threading.Tasks;
using UnityEngine;
using System.Collections.Concurrent;
using Meta.Net.NativeWebSocket;

[Serializable]
public class CommandMessage
{
    public string type;
    public string enemyType;
    public string position;
    public string mode;
    public string text;
    public string trapId;
}

[Serializable]
public class StatusMessage
{
    public string type = "STATUS";
    public int playerHealth;
    public int activeGoblins;
    public bool screenBlocked;
}

public class HMDWebSocketClient : MonoBehaviour
{
    [Header("Direkte Verbindung (nur intern)")]
    public string serverHost = "10.155.100.61";
    public int serverPort = 3000;

    [Header("Proxy Verbindung von außen")]
    public bool useHttpsProxy = true;
    public string proxyHost = "wipro.lab.sabertooth.dev";
    // Port 443 ist Standard für wss, muss in der URL nicht extra angegeben werden

    private WebSocket websocket;

    [Header("Game systems")]
    public GoblinSpawner enemySpawner;
    public WeatherController weatherController;
    public ScreenBlocker screenBlocker;

    [Header("Status reporting")]
    public bool sendStatus = true;
    public float statusIntervalSeconds = 1.0f;
    private float statusTimer = 0f;

    public int activeGoblinsManual = 0;
    public PlayerHealth playerHealth;

    private ConcurrentQueue<string> messageQueue = new ConcurrentQueue<string>();
    public GameDirector director;

    async void Start()
    {
        string url;

        if (useHttpsProxy)
        {
            // Verbindung über Nginx Proxy mit SSL
            url = $"wss://{proxyHost}/?role=hmd";
        }
        else
        {
            // Direkte Verbindung ins LAN
            url = $"ws://{serverHost}:{serverPort}/?role=hmd";
        }

        Debug.Log($"HMD WebSocket verbindet zu {url}");

        websocket = new WebSocket(url);

        websocket.OnOpen += () =>
        {
            Debug.Log("WebSocket HMD verbunden");
        };

        websocket.OnError += (e) =>
        {
            Debug.LogError("WebSocket HMD Fehler " + e);
        };

        websocket.OnClose += (code) =>
        {
            Debug.Log("WebSocket HMD geschlossen Code " + code);
        };

        websocket.OnMessage += OnWebSocketMessage;

        await Connect();
    }

    private void OnWebSocketMessage(byte[] data, int offset, int length)
    {
        try
        {
            if (data == null || data.Length == 0)
            {
                Debug.LogWarning("Leere WebSocket Nachricht empfangen");
                return;
            }

            string message = System.Text.Encoding.UTF8.GetString(data, offset, length);
            Debug.Log("Command erhalten " + message);

            messageQueue.Enqueue(message);
        }
        catch (Exception ex)
        {
            Debug.LogError("OnWebSocketMessage Exception " + ex);
        }
    }

    void Update()
    {
        while (messageQueue.TryDequeue(out string message))
        {
            HandleCommand(message);
        }

        if (sendStatus)
        {
            statusTimer += Time.deltaTime;
            if (statusTimer >= statusIntervalSeconds)
            {
                statusTimer = 0f;
                SendStatus();
            }
        }
    }

    async Task Connect()
    {
        try
        {
            await websocket.Connect();
        }
        catch (Exception ex)
        {
            Debug.LogError("WebSocket Connect Exception " + ex.Message);
        }
    }

    async void OnApplicationQuit()
    {
        if (websocket != null)
        {
            try
            {
                await websocket.Close();
            }
            catch (Exception ex)
            {
                Debug.LogError("WebSocket Close Exception " + ex.Message);
            }
        }
    }

    void HandleCommand(string json)
    {
        Debug.Log("HandleCommand raw JSON " + json);

        CommandMessage cmd;
        try
        {
            cmd = JsonUtility.FromJson<CommandMessage>(json);
        }
        catch (Exception ex)
        {
            Debug.LogError("Fehler beim Parsen des Commands " + ex);
            return;
        }

        if (cmd == null || string.IsNullOrEmpty(cmd.type))
        {
            Debug.LogWarning("Leerer oder ungültiger Command");
            return;
        }

        Debug.Log($"Parsed Command type={cmd.type}, enemyType={cmd.enemyType}, position={cmd.position}, mode={cmd.mode}");

        if (director != null)
        {
            director.Execute(cmd);
        }
        else
        {
            Debug.LogWarning("GameDirector Referenz im HMDWebSocketClient fehlt");
        }
    }

    void SendStatus()
    {
        if (websocket == null)
            return;

        var status = new StatusMessage();

        if (playerHealth != null)
        {
            status.playerHealth = Mathf.RoundToInt(playerHealth.CurrentHealth);
        }

        status.activeGoblins = activeGoblinsManual;

        if (screenBlocker != null)
        {
            status.screenBlocked = screenBlocker.IsBlocked;
        }

        string json = JsonUtility.ToJson(status);
        Debug.Log("Sende Status " + json);

        try
        {
            _ = websocket.SendText(json);
        }
        catch (Exception ex)
        {
            Debug.LogError("SendStatus Exception " + ex);
        }
    }
}
