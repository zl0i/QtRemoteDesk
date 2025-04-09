import { Router } from 'express';
import HttpErrorHandler, { NotFoundError } from '../httpErrorHandler';
import { roomManager } from "../../src/roommanager";


const WS_HOST = process.env["WS_HOST"] || "localhost";
const WS_PORT = process.env["WS_PORT"] || 8080;


const router = Router();

router.get("/:code", (req, res) => {
    try {
        const { code } = req.params;

        if (!roomManager.hasRoom(code)) {
            throw new NotFoundError('room not found');
        }

        res.json({
            ws_video: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/video`,
            ws_events: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/events`
        });
    } catch (error) {
        HttpErrorHandler.handle(error, res);
    }
});

router.post("/", (_req, res) => {
    try {
        const code = roomManager.createRoom()
        res.json({
            code: code,
            ws_video: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/video`,
            ws_events: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/events`
        });
    } catch (error) {
        HttpErrorHandler.handle(error, res);
    }
});


export default router;