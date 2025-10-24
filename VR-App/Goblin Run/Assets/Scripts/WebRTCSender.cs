using UnityEngine;
using Unity.WebRTC;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Collections;
using System;

public class WebRTCSender : MonoBehaviour
{
  [Header("Camera and Stream dimensions")]
  [SerializeField] private OVRCameraRig cam;
  [SerializeField] private int streamWidth = 1080;
  [SerializeField] private int streamHeight = 720;

  [Header("Passthrough")]
  [SerializeField] private OVRPassthroughLayer passthroughLayer;
  [SerializeField] private RenderTexture outputTexture;

  private RTCPeerConnection peerConnection;
  private MediaStreamTrack videoTrack;
  private ClientWebSocket webSocket;

  private CancellationTokenSource cts = new CancellationTokenSource();

  IEnumerator Start()
  {
    StartCoroutine(WebRTC.Update());

    yield return ConnectWebSocket();

    var config = new RTCConfiguration
    {
      iceServers = new RTCIceServer[]
        {
                new RTCIceServer { urls = new string[] { "stun:stun.l.google.com:19302" } }
        }
    };
    peerConnection = new RTCPeerConnection(ref config);

    peerConnection.OnIceCandidate = candidate =>
    {
      Debug.Log("ICE Candidate: " + candidate.Candidate);
      string json = JsonUtility.ToJson(new
      {
        type = "candidate",
        candidate = new
        {
          candidate = candidate.Candidate,
          sdpMid = candidate.SdpMid,
          sdpMLineIndex = candidate.SdpMLineIndex
        }
      });
      _ = SendStringAsync(json);
    };

    passthroughLayer.enabled = true;
    yield return new  WaitForSeconds(2);
    videoTrack = cam.rightEyeCamera.CaptureStreamTrack(streamWidth, streamHeight);
    peerConnection.AddTrack(videoTrack);

    var offerOp = peerConnection.CreateOffer();
    yield return offerOp;
    var desc = offerOp.Desc;

    var setLocalOp = peerConnection.SetLocalDescription(ref desc);
    yield return setLocalOp;

    string sdpJson = JsonUtility.ToJson(new { type = "offer", sdp = desc.sdp });
    _ = SendStringAsync(sdpJson);

    Debug.Log("Offer sent to signaling server");

    _ = Task.Run(ReceiveLoop);
  }

  private IEnumerator ConnectWebSocket()
  {
    webSocket = new ClientWebSocket();
    Uri uri = new Uri("ws://localhost:8080");

    Debug.Log("Connecting to signaling server...");
    var connectTask = webSocket.ConnectAsync(uri, CancellationToken.None);
    while (!connectTask.IsCompleted) yield return null;

    if (webSocket.State == WebSocketState.Open)
    {
      Debug.Log("Connected to signaling server!");

      // Identify as Unity peer
      string hello = JsonUtility.ToJson(new { role = "unity" });
      _ = SendStringAsync(hello);
    }
    else
    {
      Debug.LogError("Failed to connect to signaling server.");
    }
  }

  private async Task SendStringAsync(string message)
  {
    if (webSocket == null || webSocket.State != WebSocketState.Open)
    {
      Debug.LogError("WebSocket is not connected.");
      return;
    }

    byte[] buffer = Encoding.UTF8.GetBytes(message);
    await webSocket.SendAsync(
        new ArraySegment<byte>(buffer),
        WebSocketMessageType.Text,
        true,
        CancellationToken.None);
  }

  private async Task ReceiveLoop()
  {
    byte[] buffer = new byte[8192];
    while (webSocket.State == WebSocketState.Open && !cts.Token.IsCancellationRequested)
    {
      var result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), cts.Token);
      if (result.MessageType == WebSocketMessageType.Close)
      {
        Debug.Log("WebSocket closed by remote peer");
        break;
      }

      string msg = Encoding.UTF8.GetString(buffer, 0, result.Count);
      HandleSignalingMessage(msg);
    }
  }

  private void HandleSignalingMessage(string msg)
  {
    Debug.Log("Received signaling message: " + msg);
    var msgObj = JsonUtility.FromJson<SignalingMessage>(msg);

    if (msgObj.type == "answer")
    {
      StartCoroutine(HandleAnswer(msgObj.sdp));
    }
    else if (msgObj.type == "candidate")
    {
      try
      {
        var candInit = JsonUtility.FromJson<RTCIceCandidateInit>(msgObj.candidate.ToString());
        var candidate = new RTCIceCandidate(candInit);
        peerConnection.AddIceCandidate(candidate);
      }
      catch (Exception e)
      {
        Debug.LogError($"Failed to parse ICE candidate: {e}");
      }
    }
  }

  private IEnumerator HandleAnswer(string sdp)
  {
    var answer = new RTCSessionDescription
    {
      type = RTCSdpType.Answer,
      sdp = sdp
    };

    var setRemoteOp = peerConnection.SetRemoteDescription(ref answer);
    yield return setRemoteOp;

    if (setRemoteOp.IsError)
      Debug.LogError($"SetRemoteDescription failed: {setRemoteOp.Error.message}");
    else
      Debug.Log("Remote SDP set successfully!");
  }

  private void Update()
  {
    WebRTC.Update();
  }

  private void OnDestroy()
  {
    cts.Cancel();
    videoTrack?.Dispose();
    peerConnection?.Close();
    peerConnection?.Dispose();
    webSocket?.Dispose();
  }

  private async void OnApplicationQuit()
  {
    cts.Cancel();
    if (webSocket != null && webSocket.State == WebSocketState.Open)
      await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Bye", CancellationToken.None);

    peerConnection?.Close();
  }

  [Serializable]
  public class SignalingMessage
  {
    public string type;
    public string sdp;
    public object candidate;
  }
}
