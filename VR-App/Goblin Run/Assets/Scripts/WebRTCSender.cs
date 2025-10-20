using UnityEngine;
using Unity.WebRTC;
using UnityEngine.XR.Interaction.Toolkit.Inputs;
using System.Collections.Generic;
using System.Collections;



public class WebRTCSender : MonoBehaviour
{
  [Header("Camera and Stream dimensions")]
  [SerializeField] private Camera cam;
  [SerializeField] private int streamWidth = 1080;
  [SerializeField] private int streamHeight = 720;

  [Header("Passthrough")]
  [SerializeField] private OVRPassthroughLayer passthroughLayer;
  [SerializeField] private RenderTexture outputTexture;
  private RTCPeerConnection peerConnection;
  private MediaStreamTrack videoTrack;

  IEnumerator Start()
  {
    StartCoroutine(WebRTC.Update());
    var config = new RTCConfiguration
    {
      iceServers = new RTCIceServer[]
        {
                // free public STUN server hosted by Google
                new RTCIceServer { urls = new string[] { "stun:stun.l.google.com:19302" } }
        }
    };
    peerConnection = new RTCPeerConnection(ref config);

    passthroughLayer.enabled = true;
    // cam.targetTexture = outputTexture;
    //videoTrack = new VideoStreamTrack(outputTexture);
    videoTrack = cam.CaptureStreamTrack(streamWidth, streamHeight);

    peerConnection.AddTrack(videoTrack);

    peerConnection.AddTrack(videoTrack);

    var offerOp = peerConnection.CreateOffer();
    yield return offerOp;

    if (offerOp.IsError)
    {
      Debug.LogError($"Offer failed: {offerOp.Error.message}");
      yield break;
    }

    var desc = offerOp.Desc;
    var setLocalOp = peerConnection.SetLocalDescription(ref desc);
    yield return setLocalOp;

    if (setLocalOp.IsError)
    {
      Debug.LogError($"SetLocalDescription failed: {setLocalOp.Error.message}");
      yield break;
    }

    Debug.Log("Local SDP offer:\n" + desc.sdp);
    // TODO: Send desc.sdp to the remote peer via signaling
    // // Suppose you have a WebSocket connection to your signaling server
    //webSocket.Send(JsonUtility.ToJson(new
    //{
    //    type = "offer",
    //    sdp = desc.sdp
    //}));
  }

  void Update()
  {

  }

  private void OnDestroy()
  {
    videoTrack?.Dispose();
    peerConnection?.Close();
    peerConnection?.Dispose();
  }
}
/*
Example for how to receive offer via signaling channel (websocket)
-----------------------------------------------------------------

<video id="remoteVideo" autoplay playsinline></video>
<script>
const pc = new RTCPeerConnection();

// Display the incoming video track
pc.ontrack = event => {
  const video = document.getElementById('remoteVideo');
  video.srcObject = event.streams[0];
};

// Assume you received Unity's offer over WebSocket or HTTP
async function handleOffer(unityOffer) {
  const desc = new RTCSessionDescription({
    type: 'offer',
    sdp: unityOffer.sdp
  });

  await pc.setRemoteDescription(desc);

  // Create the answer
  const answer = await pc.createAnswer();
  await pc.setLocalDescription(answer);

  // Send the answer SDP back to Unity via signaling
  signalingSocket.send(JSON.stringify({
    type: 'answer',
    sdp: answer.sdp
  }));
}
</script>
*/
