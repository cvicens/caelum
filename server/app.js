// const TOKEN = process.env.TELEGRAM_TOKEN;
// const DEV_MODE = process.env.DEV_MODE || true;
// const url = process.env.ROUTE_URL;

const express = require('express');
const json = require('body-parser');
// console.log(`DEV: ${DEV_MODE}`);

const app = express();

// parse the updates to JSON
app.use(json());

// Logging pings...
app.use(function (req, res, next) {
    const fullUrl = `${req.protocol}://${req.get('host')}${req.originalUrl}`;
    console.log(`>>> ${fullUrl} Request: ${JSON.stringify(req.body)}`);
    next();
});

// We are receiving updates at the route below!
app.get(`/info`, (req, res) => {
    res.sendStatus(200).json({ result: 'SUCCESS', msg: 'dummy' });
});

// Init
const db = require('./db');
const mqtt = require('./mqtt');

const ACC_MASK = 1; // 0001
const BAT_MASK = 2; // 0010
const CO2_MASK = 4; // 0100
const GPS_MASK = 8; // 1000

// const customersRoute = require('./lib/routes/customers');

// app.use('/api', customersRoute);

mqtt.connect(
    (topic, raw) => {
        let message = JSON.parse(raw);
        console.debug(`Topic: is: ${topic}`);
        // console.debug(`Message is:\n ${JSON.stringify(message)}`);
        let columns = [];
        let values = [];

        columns.push('RSSI');
        columns.push('SNR');
        values.push(message.uplink_message.rx_metadata[0].rssi);
        values.push(message.uplink_message.rx_metadata[0].snr);

        if (message.received_at) {
            columns.push('RECEIVED_AT');
            values.push(new Date(message.received_at).toISOString());
        }
        
        if ((message.uplink_message.f_port & ACC_MASK) === ACC_MASK) {
            columns.push('ACCELERATION');
            values.push(message.uplink_message.decoded_payload.accelAbs);
        }
        if ((message.uplink_message.f_port & BAT_MASK) === BAT_MASK) {
            columns.push('VBAT');
            values.push(message.uplink_message.decoded_payload.vBat);
        }
        if ((message.uplink_message.f_port & CO2_MASK) === CO2_MASK) {
            columns.push('CO2');
            columns.push('TEMPERATURE');
            columns.push('HUMIDITY');
            values.push(message.uplink_message.decoded_payload.co2);
            values.push(message.uplink_message.decoded_payload.temperature);
            values.push(message.uplink_message.decoded_payload.humidity);
        }
        if ((message.uplink_message.f_port & GPS_MASK) === GPS_MASK) {
            columns.push('LATITUDE');
            columns.push('LAT');
            columns.push('LONGITUDE',);
            columns.push('LON');
            columns.push('FQ');
            columns.push('SATELLITES');
            values.push(message.uplink_message.decoded_payload.latitude);
            values.push(message.uplink_message.decoded_payload.lat);
            values.push(message.uplink_message.decoded_payload.longitude);
            values.push(message.uplink_message.decoded_payload.lon);
            values.push(message.uplink_message.decoded_payload.fq);
            values.push(message.uplink_message.decoded_payload.satellites);
        }
        
        db.insert('METRICS', columns, values)
        .then(function (data) {
            console.log(`DATA: ${JSON.stringify(data)}`);
        })
        .catch(function (err) {
            console.log(`ERROR: ${JSON.stringify(err)}`);
        });;
    },
    (error) => {
        console.error("error is " + error);
        db.close()
        .then(function (data) {
            console.log(JSON.stringify(data));
        })
        .catch(function (err) {
            console.log(JSON.stringify(err));
        });
    }
);

// Start Express Server
const port = process.env.PORT || process.env.CAELUM_CUSTOM_PORT || 3000;
const host = process.env.IP || process.env.CAELUM_CUSTOM_HOST || '0.0.0.0';
app.listen(port, host, () => {
    console.log(`HTTP Sever started at: ${new Date()} on port: ${port}`);
});

console.log('Caelum Server has started!');