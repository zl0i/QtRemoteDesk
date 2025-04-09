import WebSocket, { WebSocketServer } from 'ws';
import { type Room, roomManager } from './roommanager';

const API_TOKEN = process.env["API_TOKEN"] || "secret";
const WS_PORT = Number(process.env["WS_PORT"]) || 8080;

enum Event {
    FirstVideoReceiverConnected = 1501,
    NewVideoReceiverConnected = 1502,
    LastVideoReceiverDisconnected = 1503,
    EventReceiverConnected = 1504,
    EventReceiverDisconnected = 1505
}


class RoomsServer {
    private wsServer: WebSocketServer = new WebSocketServer({ port: WS_PORT });
    constructor() {

    }

    private sendToRecivers(room: Room) {
        return (message: WebSocket.RawData) => {
            room.lastActivity = Date.now();
            room.videoReceivers.forEach((receiver) => {
                if (receiver.readyState === WebSocket.OPEN) {
                    receiver.send(message);
                }
            });
        }
    }

    private sendEventToReciver(room: Room) {
        return (message: WebSocket.RawData) => {
            room.lastActivity = Date.now();
            room.eventReceiver?.send(message);
        }
    }

    private sendEventToSender(room: Room) {
        return (message: WebSocket.RawData) => {
            room.lastActivity = Date.now();
            room.eventFromSender?.send(message);
        }
    }

    private closeRoom(room: Room) {
        room.videoFromSender?.close();
        room.videoReceivers.forEach((receiver) => {
            receiver.close();
        });
        room.eventFromSender?.close();
        room.eventReceiver?.close();
        roomManager.deleteRoom(room.code);
    }



    listen(cb: () => void) {
        this.wsServer.on('connection', (ws, req) => {
            if (!req.url) {
                ws.close();
                return;
            }
            if (req.headers.authorization !== `Bearer ${API_TOKEN}`) {
                ws.close();
                return;
            }


            const paths = req.url.split('/');
            const roomCode = paths[2]
            const typeConnect = paths[3] //events or video
            const typeWS = paths[4] //sender or receiver

            const room = roomManager.getRoom(roomCode);
            if (!room) {
                ws.close();
                return;
            }

            if (typeConnect === 'video') {
                if (typeWS === 'sender') {
                    console.log(new Date().toISOString(), `Video sender connected for room ${room.code}`);
                    room.videoFromSender = ws;
                    ws.on('message', this.sendToRecivers(room));
                    ws.on('close', () => this.closeRoom(room));
                } else if (typeWS === 'receiver') {
                    console.log(new Date().toISOString(), `Video receiver connected for room ${room.code}`);
                    room.videoReceivers.add(ws);
                    if (room.videoReceivers.size === 1) {
                        const encoder = new TextEncoder();
                        const binaryData = encoder.encode(JSON.stringify({ type: Event.FirstVideoReceiverConnected, code: room.code }));
                        room.eventFromSender?.send(binaryData);
                    }
                    ws.on('close', () => {
                        room.videoReceivers.delete(ws);
                        if (room.videoReceivers.size === 0) {
                            const encoder = new TextEncoder();
                            const binaryData = encoder.encode(JSON.stringify({ type: Event.LastVideoReceiverDisconnected, code: room.code }));
                            room.eventFromSender?.send(binaryData);
                        }
                    });
                }
            } else if (typeConnect === 'events') {
                if (typeWS === 'sender') {
                    console.log(new Date().toISOString(), `Event sender connected for room ${room.code}`);
                    room.eventFromSender = ws;
                    ws.on('message', this.sendEventToReciver(room));
                } else if (typeWS === 'receiver') {
                    console.log(new Date().toISOString(), `Event receiver connected for room ${room.code}`);
                    room.eventReceiver = ws;
                    ws.on('message', this.sendEventToSender(room));
                    ws.on('close', () => {
                        const encoder = new TextEncoder();
                        const binaryData = encoder.encode(JSON.stringify({ type: Event.EventReceiverDisconnected, code: room.code }));
                        room.eventFromSender?.send(binaryData);
                    });
                    const encoder = new TextEncoder();
                    const binaryData = encoder.encode(JSON.stringify({ type: Event.EventReceiverConnected, code: room.code }));
                    room.eventFromSender?.send(binaryData);
                }
            } else {
                ws.close();
                return;
            }
        });
        cb();
    }
}




const roomsServer = new RoomsServer();
export { roomsServer }