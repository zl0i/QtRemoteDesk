import { api } from "./src/api";
import { roomsServer } from "./src/roomsserver";



api.listen(api.get('port'), () => console.log("Http server running on ports 3000"));
roomsServer.listen(() => console.log("WebSocket server running on port 8080"))