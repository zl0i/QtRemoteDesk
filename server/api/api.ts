import express from "express";
import morgan from "morgan";
import RoomRouter from "./rooms/router";
import * as prom from './middleware/prometheus';
import { auth } from "./middleware/auth";

const HTTP_PORT = process.env["HTTP_PORT"] || 3000;


const api = express();

api.use(express.json());
api.use(express.urlencoded({ extended: false }));
api.set('port', HTTP_PORT);
api.get('/healthchecks', (_req, res) => { res.end('ok'); });
api.get('/metrics', prom.metricsRoute);

api.use(prom.middleware);
api.use(morgan(':date[iso] :req[x-forwarded-for] :method :url :status :response-time ms'));

api.use(auth());
api.use('/rooms', RoomRouter);

export { api }