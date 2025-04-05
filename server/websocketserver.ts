import WebSocket, { WebSocketServer } from 'ws';
import { type Room, roomManager } from './roommanager';

const WS_PORT = process.env["WS_PORT"] || 8080;



class RoomsServer {
    private wsServer: WebSocketServer = new WebSocketServer({ port: WS_PORT });
    constructor() {

    }

    private sendToRecivers(room: Room) {
        return (message: WebSocket.RawData) => {
            room.videoReceivers.forEach((receiver) => {
                if (receiver.readyState === WebSocket.OPEN) {
                    receiver.send(message);
                }
            });
        }
    }

    private sendEventToReciver(room: Room) {
        return (message: WebSocket.RawData) => {
            room.eventReceiver?.send(message);
        }
    }

    private sendEventToSender(room: Room) {
        return (message: WebSocket.RawData) => {
            room.eventFromSender?.send(message);
        }
    }

    listen(cb: () => void) {
        this.wsServer.on('connection', (ws, req) => {
            if (!req.url) {
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
                    console.log('Video sender connected');
                    room.videoFromSender = ws;
                    ws.on('message', this.sendToRecivers(room));
                } else if (typeWS === 'receiver') {
                    console.log('Video receiver connected');
                    room.videoReceivers.add(ws);
                }
            } else if (typeConnect === 'events') {
                if (typeWS === 'sender') {
                    console.log('Event sender connected');
                    room.eventFromSender = ws;
                    ws.on('message', this.sendEventToReciver(room));
                } else if (typeWS === 'receiver') {
                    console.log('Event receiver connected');
                    room.eventReceiver = ws;
                    ws.on('message', this.sendEventToSender(room));
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