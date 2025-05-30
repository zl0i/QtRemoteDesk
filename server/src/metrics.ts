
import client from 'prom-client';

const register = new client.Registry();

const collectDefaultMetrics = client.collectDefaultMetrics;
collectDefaultMetrics({ register, prefix: 'remoteaccess_' });

export const httpRequestDurationMicroseconds = new client.Histogram({
    name: 'remoteaccess_http_request_duration_seconds',
    help: 'Duration of HTTP requests in microseconds',
    labelNames: ['method', 'route', 'code'],
    buckets: [0.1, 0.3, 0.5, 0.7, 1, 3, 5, 7, 10]
});

export const roomstGauge = new client.Gauge({
    name: 'remoteaccess_rooms_gauge',
    help: 'Count of Rooms',
    labelNames: []
});

register.registerMetric(httpRequestDurationMicroseconds);
register.registerMetric(roomstGauge);

export { register };