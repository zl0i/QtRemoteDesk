import express from 'express';

const API_TOKEN = process.env["API_TOKEN"] || "secret";

export function auth() {
    return (req: express.Request, res: express.Response, next: express.NextFunction) => {
        if (req.headers.authorization !== `Bearer ${API_TOKEN}`) {
            res.status(401).json({ error: "Unauthorized" });
        } else {
            next();
        }
    }
}