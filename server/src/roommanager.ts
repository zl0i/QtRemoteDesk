import type WebSocket from 'ws';
import { roomstGauge } from './metrics';

export interface Room {
    code: string;
    videoFromSender?: WebSocket;
    eventFromSender?: WebSocket;
    videoReceivers: Set<WebSocket>;
    eventReceiver?: WebSocket,
    createdAt: number;
    lastActivity: number;
}

class RoomManager {

    private rooms = new Map(); // roomCode -> Room
    constructor() { }

    generateRoomCode() {
        const letters = '0123456789';
        let code = '';

        do {
            for (let i = 0; i < 6; i++) {
                code += letters[Math.floor(Math.random() * letters.length)];
            }
        } while (this.rooms.has(code));
        return code;
    }

    createRoom() {
        const roomCode = this.generateRoomCode();
        const room: Room = {
            code: roomCode,
            videoReceivers: new Set(),
            createdAt: Date.now(),
            lastActivity: Date.now()
        };
        this.rooms.set(roomCode, room);
        roomstGauge.inc();
        return roomCode;
    }

    hasRoom(roomCode: string): boolean {
        return this.rooms.has(roomCode);
    }
    getRoom(roomCode: string): Room | undefined {
        return this.rooms.get(roomCode);
    }

    deleteRoom(roomCode: string) {
        roomstGauge.dec();
        this.rooms.delete(roomCode);
    }

    cleanupInactiveRooms() {
        const now = Date.now();
        for (const [code, room] of this.rooms.entries()) {
            if (now - room.lastActivity > 1 * 60 * 60 * 1000 || now - room.createdAt > 24 * 60 * 60 * 1000) { // 1 hour inactivity or 24 hours since creation
                room.videoFromSender?.close();
                room.videoReceivers.forEach((receiver: WebSocket) => {
                    receiver.close();
                });
                room.eventFromSender?.close();
                room.eventReceiver?.close();
                this.rooms.delete(code);
            }
        }
    }
}

setInterval(() => roomManager.cleanupInactiveRooms(), 60000);
const roomManager = new RoomManager();
export { roomManager }