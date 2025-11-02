const WebSocket = require('ws');
const { WebSocketServer } = WebSocket;

const wss = new WebSocketServer({ host: '0.0.0.0', port: 8080 });

let unitySocket = null;
let browserSocket = null;

const green = '\x1b[32m%s\x1b[0m';
const blue = '\x1b[34m%s\x1b[0m';
const yellow = '\x1b[33m%s\x1b[0m';

wss.on('connection', ws => {
  console.log(yellow, 'New connection received...');

  ws.on('message', message => {
    console.log(yellow, 'Message received:', message.toString()); // log raw message
    const msg = JSON.parse(message);

    if (msg.role === 'unity') {
      unitySocket = ws;
      console.log(green, '✅ Unity connected');
      return;
    }
    if (msg.role === 'browser') {
      browserSocket = ws;
      console.log(blue, '🌐 Browser connected');
      return;
    }

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
    console.log(yellow, 'Connection closed');
  });
});

console.log('Signaling server running on ws://192.168.137.1:8080');
