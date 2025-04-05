const WebSocket = require('ws');
const http = require('http');

// Сервер для видеопотока (порт 8080)
const wssVideo = new WebSocket.Server({ port: 8080 });
let videoSender = null;
const videoReceivers = new Set();

wssVideo.on('connection', (ws) => {
  ws.on('message', (message) => {
    if (message.toString() === 'sender') {
      videoSender = ws;
      console.log('Video sender connected');
      ws.on('close', () => {
        videoSender = null;
        console.log('Video sender disconnected');
      });
    } else if (message.toString() === 'receiver') {
      videoReceivers.add(ws);
      console.log('Video receiver connected');
      ws.on('close', () => {
        videoReceivers.delete(ws);
        console.log('Video receiver disconnected');
      });
    } else if (ws === videoSender) {
      videoReceivers.forEach((receiver) => {
        if (receiver.readyState === WebSocket.OPEN) {
          receiver.send(message);
        }
      });
    }
  });
});

// Сервер для событий (порт 8081)
// const wssEvents = new WebSocket.Server({ port: 8081 });
// let eventSender = null;
// const eventReceivers = new Set();

// wssEvents.on('connection', (ws) => {
//   ws.on('message', (message) => {
//     if (message.toString() === 'sender') {
//       eventSender = ws;
//       console.log('Event sender connected');
//       ws.on('close', () => {
//         eventSender = null;
//         console.log('Event sender disconnected');
//       });
//     } else if (message.toString() === 'receiver') {
//       eventReceivers.add(ws);
//       console.log('Event receiver connected');
//       ws.on('close', () => {
//         eventReceivers.delete(ws);
//         console.log('Event receiver disconnected');
//       });
//     } else if (ws === eventSender) {
//       eventReceivers.forEach((receiver) => {
//         if (receiver.readyState === WebSocket.OPEN) {
//           receiver.send(message);
//         }
//       });
//     }
//   });
// });

const wssEvents = new WebSocket.Server({ port: 8081 });
let eventSender = null;
let eventReceiver = null;

wssEvents.on('connection', (ws) => {
  ws.on('message', (message) => {
    if (message.toString() === 'sender') {
      eventSender = ws;
      console.log('Event sender connected');
      ws.on('close', () => {
        eventSender = null;
        console.log('Event sender disconnected');
      });
    } else if (message.toString() === 'receiver') {
      eventReceiver = ws;
      console.log('Event receiver connected');
      ws.on('close', () => {
        eventReceiver = null;
        console.log('Event receiver disconnected');
      });
    } else if (ws === eventSender && eventReceiver) {
      eventReceiver.send(message);
    } else if (ws === eventReceiver && eventSender) {
      eventSender.send(message);
    }
  });
});

console.log('Servers running on ports 8080 (video) and 8081 (events)');
