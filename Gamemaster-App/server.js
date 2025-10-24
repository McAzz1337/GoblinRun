// server.js
const WebSocket = require('ws');
const { WebSocketServer } = WebSocket;

const wss = new WebSocketServer({ port: 8080 });
let unitySocket = null;
let browserSocket = null;

wss.on('connection', ws => {
  ws.on('message', message => {
    const msg = JSON.parse(message);

    // Identify who is who
    if (msg.role === 'unity') {
      unitySocket = ws;
      console.log('Unity connected');
    } else if (msg.role === 'browser') {
      browserSocket = ws;
      console.log('Browser connected');
    }

    // Forward messages between Unity and Browser
    if (unitySocket && browserSocket) {
      if (ws === unitySocket && browserSocket.readyState === WebSocket.OPEN) {
        browserSocket.send(message);
      } else if (ws === browserSocket && unitySocket.readyState === WebSocket.OPEN) {
        unitySocket.send(message);
      }
    }
  });

  ws.on('close', () => {
    if (ws === unitySocket) unitySocket = null;
    if (ws === browserSocket) browserSocket = null;
  });
});

console.log('Signaling server running on ws://localhost:8080');
