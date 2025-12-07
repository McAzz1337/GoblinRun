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

## Scripts
Note: 
The CLI commands can be found in the directory: /Scripts/cli_commands.txt
The whole vendor folder with the dependencies together with the models and texture forlders for the Geo-App was provided to the experts.
Place these folders in the same folder the as the src folder which contains the source code.

# dhm2objawk.sed
Run this one on Linux, WSL or the Git Bash command line
Paste x and y min and max into the script from:
https://map.geo.admin.ch/#/map?lang=en&center=2683887,1216076.63&z=6.837&topic=ech&layers=ch.swisstopo.zeitreihen@year=1864,f;ch.bfs.gebaeude_wohnungs_register,f;ch.bav.haltestellen-oev,f;ch.swisstopo.swisstlm3d-wanderwege,f;ch.vbs.schiessanzeigen,f;ch.astra.wanderland-sperrungen_umleitungen,f&bgLayer=ch.swisstopo.pixelkarte-farbe&featureInfo=default
Careful! The ymin has to be the smaller one and ymax the larger
The height info will be extracted from the asc file specified
The asc file can be found on: https://www.swisstopo.admin.ch/en/height-models/dhm25.
This file is also provided to the experts of the project, since if something changed in the file provided online in the meantime
the script and potentially the C++ application has to be adjusted.

# map_image.py
Paste the same coordinates from the above script in this one
This script will then download the image of this area (with a small margin, if the image is a little bit off, remove or adjust said margin)
Images will be downloaded from: "https://wms.geo.admin.ch/"

# premake
This will only work on Windows. It generates a Visual Studio Solution.
Open the folder the premake.lua script is in and run: .\premake5.exe vs<XXXX> where XXXX is the version of Visual Studio used.
Run .\premake5.exe --help to see what versions of Visual Studio Solutions can be generated.
When using MacOs or Linux you will have to pass the Libraries and Preprocessor defines used to the compiler (easiest done with a Makefile
and ran by make).
In the premake.lua script all the info about what dependencies the project uses are listed together with the preprocessor defines.

# Geo-APP
Create a new config file like the one in the config folder.
The field image can be omitted. The application will just render the map model in gray when the image is not specified in the config.


# GPS-Sender-App
This is an Android app that just lets the user enter an IP address and when the start button is pressed it will start sending
the GPS coordinates in intervals to that IP. To test if this works for a location that differs from the phones real 
location the GPS-location can be spoofed by using another App available from the Appstore/Google play store.