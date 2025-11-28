#!/usr/bin/env python3
"""
Download a SwissTopo map portion given CH1903 / LV03 coordinates.
"""

import requests
from PIL import Image
from io import BytesIO

# -----------------------------
# 1️⃣ Enter your CH1903 bounds
# -----------------------------
xmin = 751486
xmax = 754990
ymin = 188042
ymax = 190007

# Optional: add a small margin to avoid cropping
margin = 50
xmin -= margin
xmax += margin
ymin -= margin
ymax += margin

# -----------------------------
# 2️⃣ Conversion CH1903 → WGS84
# -----------------------------
def ch1903_to_wgs84(E, N):
    # formulas from https://geodesy.geo.admin.ch
    E_aux = (E - 600000) / 1000000.0
    N_aux = (N - 200000) / 1000000.0

    lat = (
        16.9023892
        + 3.238272 * N_aux
        - 0.270978 * E_aux**2
        - 0.002528 * N_aux**2
        - 0.0447 * E_aux**2 * N_aux
        - 0.0140 * N_aux**3
    )
    lon = (
        2.6779094
        + 4.728982 * E_aux
        + 0.791484 * E_aux * N_aux
        + 0.1306 * E_aux * N_aux**2
        - 0.0436 * E_aux**3
    )

    lat = lat * 100.0 / 36.0
    lon = lon * 100.0 / 36.0
    return lat, lon

sw_lat, sw_lon = ch1903_to_wgs84(xmin, ymin)
ne_lat, ne_lon = ch1903_to_wgs84(xmax, ymax)

# -----------------------------
# 3️⃣ Download map image
# -----------------------------
url = "https://wms.geo.admin.ch/"
params = {
    "SERVICE": "WMS",
    "REQUEST": "GetMap",
    "VERSION": "1.3.0",
    "LAYERS": "ch.swisstopo.pixelkarte-farbe",
    "STYLES": "",
    "CRS": "EPSG:4326",
    "BBOX": f"{sw_lat},{sw_lon},{ne_lat},{ne_lon}",  # WGS84
    "WIDTH": 1024,
    "HEIGHT": 1024,
    "FORMAT": "image/png",
}

print("Downloading map image...")
response = requests.get(url, params=params)
response.raise_for_status()

img = Image.open(BytesIO(response.content))
img.save("map.png")
print("Saved map.png")
