import { api } from "./api";
import { roomsServer } from "./roomsserver";



api.listen(3000, () => console.log("Http server running on ports 3000"));
roomsServer.listen(() => console.log("WebSocket server running on port 8080"))