## GoblinRun

# Plugins  
OpenXr<br>
Unity OpenXr Meta<br>
Xr Interaction Toolkit<br>
Meta Xr Core SDK (from asset store)<br>
WebRTC (see below)<br>
MetaXR Simulator (for emulating camera passthrough emulation for now)<br>

For WebRTC open `ProjectDirectory/Packages/manifest.json`<br>
Add this line: `"com.unity.webrtc": "3.0.0-exp.6"`<br>
Then in Unity open the Package Manager and search for WebRTC in the Unity Registry and click upgrade to `3.0.0-pre.8`<br>
This will add the pre-release of the WebRTC package to the project <br>

# Inital setup
https://developers.meta.com/horizon/documentation/unity/unity-project-setup <br>
https://developers.meta.com/horizon/documentation/unity/unity-tutorial-hello-vr <br>  

# Geo App
Because of problems with merging the geo branch into main, due to git complaining about unrelated git history
it is advised to clone the geo branch separately for the newest changes (these are minor changes).
