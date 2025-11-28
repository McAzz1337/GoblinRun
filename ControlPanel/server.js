const express = require("express");
const http = require("http");
const WebSocket = require("ws");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

app.use(express.static("public"));

let hmdClient = null;
const controllerClients = new Set();

wss.on("connection", function connection(ws, req) {
  const url = req.url || "";
  console.log("Neue Verbindung", url);

  if (url.includes("role=hmd")) {
    console.log("HMD connected");
    hmdClient = ws;

    ws.on("close", (code, reason) => {
      console.log("HMD disconnected", code, reason?.toString());
      if (hmdClient === ws) {
        hmdClient = null;
      }
    });

    ws.on("message", (data) => {
      const text = typeof data === "string" ? data : data.toString();
      console.log("HMD message:", text);

      controllerClients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
          client.send(text);
        }
      });
    });

  } else {
    console.log("Controller verbunden");
    controllerClients.add(ws);

    ws.on("close", () => {
      console.log("Controller getrennt");
      controllerClients.delete(ws);
    });

    ws.on("message", (data) => {
      console.log("Controller Nachricht:", data.toString());
      if (hmdClient && hmdClient.readyState === WebSocket.OPEN) {
        hmdClient.send(data);
      }
    });
  }
});

const PORT = 3000;
server.listen(PORT, () => {
  console.log(`Server läuft auf Port ${PORT}`);
});
