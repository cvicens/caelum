const mqtt = require('mqtt');

const CAELUM_TOPIC = process.env.CAELUM_TOPIC || 'v3/caelum-air-quality@ttn/devices/eui-9676b6000010ab14/up';

const MQTT_HOST = process.env.MQTT_HOST || 'eu1.cloud.thethings.network';
const MQTT_PORT = process.env.MQTT_PORT || '1883';
const MQTT_CLIENTID = process.env.MQTT_CLIENTID || 'caelum-server';

const MQTT_USERNAME = process.env.MQTT_USERNAME;
const MQTT_PASSWORD = process.env.MQTT_PASSWORD;

const options = {
    host: MQTT_HOST,
    port: MQTT_PORT,
    clientId: MQTT_CLIENTID,
    username: MQTT_USERNAME,
    password: MQTT_PASSWORD,
    clean: true
};

let client = null;

function connect(onMessage, onError) {
    console.debug(`Connecting to ${JSON.stringify(options)}`)
    client = mqtt.connect(options);

    client.on('connect', () => {
        client.subscribe([CAELUM_TOPIC], (err) => {
            if (!err) {
                console.log(`Correctly subscribed to: ${CAELUM_TOPIC}`);
            } else {
                console.log(`Error when subscribing to: ${CAELUM_TOPIC}`);
            }
        });
    });

    client.on('message', onMessage);
    // client.on('message', (topic, message, packet) => {
	//     console.log("message is "+ message);
	//     console.log("topic is "+ topic);
    // });
    client.on("error", onError);
    // client.on('error', (error) => {
    //     console.log(`error: ${JSON.stringify(error)}`);
    // });
}

module.exports = {
    connect: (onMessage, onError) => {
        connect(onMessage, onError);
    }
};


