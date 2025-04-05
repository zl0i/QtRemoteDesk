import express from "express";
import morgan from "morgan";
import { roomManager } from "./roommanager";
import { roomsServer } from "./roomsserver";

const WS_HOST = process.env["WS_PORT"] || "localhost";
const WS_PORT = process.env["WS_PORT"] || 8080;

const app = express();

app.use(express.json());
app.use(express.urlencoded({ extended: false }));

app.use(morgan(':date[iso] :req[x-forwarded-for] :method :url :status :response-time ms'));
app.post("/rooms", (_req, res) => {
    const code = roomManager.createRoom()
    res.json({
        code: code,
        ws_video: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/video`,
        ws_events: `ws://${WS_HOST}:${WS_PORT}/rooms/${code}/events`
    });
});


app.get("/rooms/:code", (req, res) => {
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

app.listen(3000, () => console.log("Http server running on ports 3000"));
roomsServer.listen(() => console.log("WebSocket server running on port 8080"))