let socket = null;
const statusEl = document.getElementById("status");
const logEl = document.getElementById("log");

function log(message) {
  const time = new Date().toLocaleTimeString();
  const line = document.createElement("div");
  line.textContent = `[${time}] ${message}`;
  logEl.prepend(line);
}

function setButtonsEnabled(enabled) {
  const buttons = document.querySelectorAll("button");
  buttons.forEach(b => b.disabled = !enabled);
}

// einfache Status Anzeige
function updateStatusUi(data) {
  const healthEl = document.getElementById("statusHealth");
  const goblinsEl = document.getElementById("statusGoblins");
  const screenEl = document.getElementById("statusScreen");

  if (healthEl && typeof data.playerHealth === "number") {
    healthEl.textContent = `Health: ${data.playerHealth}`;
  }

  if (goblinsEl && typeof data.activeGoblins === "number") {
    goblinsEl.textContent = `Active Goblins: ${data.activeGoblins}`;
  }

  if (screenEl && typeof data.screenBlocked === "boolean") {
    screenEl.textContent = `Screen block: ${data.screenBlocked ? "ON" : "OFF"}`;
  }
}

// Verbindet sich als Controller
function connect() {
  const isHttps = window.location.protocol === "https:";
  const wsProtocol = isHttps ? "wss" : "ws";
  const host = window.location.host; // inkl. Port, falls gesetzt

  // Nginx leitet alles an Node weiter
  const url = `${wsProtocol}://${host}/?role=controller`;

  log(`Verbinde mit ${url}`);
  socket = new WebSocket(url);

  socket.onopen = () => {
    statusEl.textContent = "Verbunden mit Server";
    log("WebSocket offen");
    setButtonsEnabled(true);
  };

  socket.onclose = () => {
    statusEl.textContent = "Verbindung getrennt, versuche erneut in 3 Sekunden";
    log("WebSocket geschlossen");
    setButtonsEnabled(false);
    setTimeout(connect, 3000);
  };

  socket.onerror = (err) => {
    console.error("WebSocket Fehler", err);
    log("WebSocket Fehler, Details in Konsole");
  };

  socket.onmessage = (msg) => {
    log(`Nachricht vom HMD oder Server: ${msg.data}`);

    try {
      const data = JSON.parse(msg.data);
      if (data.type === "STATUS") {
        updateStatusUi(data);
      }
    } catch (e) {
      // kein JSON oder uninteressant
    }
  };
}


function sendCommand(command) {
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    log("Senden nicht möglich, keine Verbindung");
    return;
  }
  const json = JSON.stringify(command);
  log(`Sende: ${json}`);
  socket.send(json);
}

// Verbindung beim Laden aufbauen
connect();
setButtonsEnabled(false);

// Button Handler
document.getElementById("spawnGoblinFront").onclick = () => {
  sendCommand({
    type: "SPAWN_ENEMY",
    enemyType: "goblin",
    position: "FRONT"
  });
};

document.getElementById("spawnGoblinLeft").onclick = () => {
  sendCommand({
    type: "SPAWN_ENEMY",
    enemyType: "goblin",
    position: "LEFT"
  });
};

document.getElementById("spawnGoblinRight").onclick = () => {
  sendCommand({
    type: "SPAWN_ENEMY",
    enemyType: "goblin",
    position: "RIGHT"
  });
};

document.getElementById("weatherClear").onclick = () => {
  sendCommand({
    type: "SET_WEATHER",
    mode: "CLEAR"
  });
};

document.getElementById("weatherRain").onclick = () => {
  sendCommand({
    type: "SET_WEATHER",
    mode: "RAIN"
  });
};

document.getElementById("sendPing").onclick = () => {
  sendCommand({
    type: "DEBUG_MESSAGE",
    text: "Ping vom Control Panel"
  });
};

document.getElementById("screenBlockOn").onclick = () => {
  sendCommand({ type: "SCREEN_BLOCK", mode: "ON" });
};

document.getElementById("screenBlockOff").onclick = () => {
  sendCommand({ type: "SCREEN_BLOCK", mode: "OFF" });
};

document.getElementById("trapSpikeFront").onclick = () => {
  sendCommand({
    type: "TRAP",
    trapId: "SPIKE_FRONT"
  });
};

document.getElementById("trapSpikeLeft").onclick = () => {
  sendCommand({
    type: "TRAP",
    trapId: "SPIKE_LEFT"
  });
};

document.getElementById("trapSpikeRight").onclick = () => {
  sendCommand({
    type: "TRAP",
    trapId: "SPIKE_RIGHT"
  });
};

document.getElementById("spawnHeraldFront").onclick = () => {
  sendCommand({
    type: "SPAWN_ENEMY",
    enemyType: "herald",
    position: "FRONT"
  });
};