import type WebSocket from 'ws';

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
        for (let i = 0; i < 6; i++) {
            code += letters[Math.floor(Math.random() * letters.length)];
        }
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
        return roomCode;
    }

    hasRoom(roomCode: string): boolean {
        return this.rooms.has(roomCode);
    }
    getRoom(roomCode: string): Room | undefined {
        return this.rooms.get(roomCode);
    }

    cleanupInactiveRooms() {
        const now = Date.now();
        for (const [code, room] of this.rooms.entries()) {
            if (now - room.lastActivity > 3600000) { // 1 hour inactivity
                this.rooms.delete(code);
            }
        }
    }
}

const roomManager = new RoomManager();
setInterval(() => roomManager.cleanupInactiveRooms(), 60000);
export { roomManager }