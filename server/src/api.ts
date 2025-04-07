import express from "express";
import morgan from "morgan";
import { roomManager } from "./roommanager";

const HTTP_PORT = process.env["HTTP_PORT"] || 3000;
const WS_HOST = process.env["WS_PORT"] || "localhost";
const WS_PORT = process.env["WS_PORT"] || 8080;

const api = express();

api.use(express.json());
api.use(express.urlencoded({ extended: false }));
api.set('port', HTTP_PORT);

api.use(morgan(':date[iso] :req[x-forwarded-for] :method :url :status :response-time ms'));

api.get("/rooms/:code", (req, res) => {
    const { code } = req.params;

    if (!roomManager.hasRoom(code)) {
        res.status(404).json({
            error: "Room not found"
        });
    } else {
        res.json({
            ws_video: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/video`,
            ws_events: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/events`
        });
    }
});

api.post("/rooms", (_req, res) => {
    const code = roomManager.createRoom()
    res.json({
        code: code,
        ws_video: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/video`,
        ws_events: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/events`
    });
});

export { api }