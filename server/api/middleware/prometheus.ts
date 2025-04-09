import express from 'express';
import { register, httpRequestDurationMicroseconds } from '../../src/metrics';

export function middleware(req: express.Request, res: express.Response, next: express.NextFunction) {
    const end = httpRequestDurationMicroseconds.startTimer();
    const path = req.path;
    next();
    res.on('finish', () => {
        end({ route: path, code: res.statusCode, method: req.method });
    });
}

export async function metricsRoute(_req: express.Request, res: express.Response,) {
    res.setHeader('Content-Type', register.contentType);
    res.end(await register.metrics());
}