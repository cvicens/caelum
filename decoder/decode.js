// TggZFyQ4bXT/fw==
//
// $GPRMC,023033.799,V,,,,,0.00,0.00,060180,,,N*44
// CO2: 2126       Temperature: 25.24      Humidity: 36.56
// AccX: -0.03     AccY: -0.17     AccZ: 10.00
// GyrX: 0.06      GyrX: -0.10     GyrX: -0.09
// VBat: 4.08

let data = '2QcZQSJXAB4ECw==';

let bytes = Buffer.from(data, 'base64');

console.log(`bytes=${toHexString(bytes)}`);

//var bytes = Buffer.from([12, 178, 4, 128, 247, 174]);
var input = {
  bytes: bytes,
  fPort: 7
}

console.log(`input=${JSON.stringify(input)}`);

function toHexString(byteArray) {
  return byteArray.reduce((output, elem) => 
    (output + ('0' + elem.toString(16)).slice(-2)),
    '');
}

function u162sf(sf2u16) {
  let integer = sf2u16[0];   // INTEGER
  let decimal = sf2u16[1]; // DECIMAL
  
  return integer + (decimal / 100);
}

function u16(u8x2) {
  return (u8x2[1] << 8) | u8x2[0];
}

function decodeUplink(input) {
  var data = {};
  var events = {
    1: "CO2",
    2: "ACCELEROMETER",
    3: "CO2+ACCELEROMETER",
    4: "BATTERY",
    5: "BATTERY+CO2",
    6: "BATTERY+ACCELEROMETER",
    7: "BATTERY+ACCELEROMETER+CO2"
  };
  data.event = events[input.fPort];
  // data.co2 = Buffer.from([input.bytes[0], input.bytes[1]]).readInt16LE(0);
  data.co2 = u16([input.bytes[0], input.bytes[1]]);
  data.temperature = u162sf([input.bytes[2], input.bytes[3]]);
  data.humidity = u162sf([input.bytes[4], input.bytes[5]]);
  data.accelAbs = u162sf([input.bytes[6], input.bytes[7]]);
  data.vBat = u162sf([input.bytes[8], input.bytes[9]]);
  var warnings = [];
  if (data.temperature < -10) {
    warnings.push("it's cold");
  }
  return {
    data: data,
    warnings: warnings
  };
}

console.log(decodeUplink(input));