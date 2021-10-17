// TggZFyQ4bXT/fw==
//
// $GPRMC,023033.799,V,,,,,0.00,0.00,060180,,,N*44
// CO2: 2126       Temperature: 25.24      Humidity: 36.56
// AccX: -0.03     AccY: -0.17     AccZ: 10.00
// GyrX: 0.06      GyrX: -0.10     GyrX: -0.09
// VBat: 4.08


// Lat: 4028.735596 
// Lon: 343.151703 
// 40.48614194939981, -3.7211273343192204

let data = 'AEYEDbYBGDoqFAIEwIoDGE4omAsCVw==';

let bytes = Buffer.from(data, 'base64');

console.log(`bytes=${toHexString(bytes)}`);

//var bytes = Buffer.from([12, 178, 4, 128, 247, 174]);
var input = {
  bytes: bytes,
  fPort: 15
}

console.log(`input=${JSON.stringify(input)}`);

function toHexString(byteArray) {
  return byteArray.reduce((output, elem) => 
    (output + ('0' + elem.toString(16)).slice(-2)),
    '');
}

/////////////////////////////// TTN CONVERTER
// GCBA (GPS,CO2,BAT,ACC)
const ACC_MASK = 1; // 0001
const BAT_MASK = 2; // 0010
const CO2_MASK = 4; // 0100
const GPS_MASK = 8; // 1000

function u162gps(u16bytes) {
  let integer = u16bytes[0];   // INTEGER
  let decimal = u16bytes[1]; // DECIMAL
  
  return integer + (decimal / 10000);
}

function u162sf(u16bytes) {
  let integer = u16bytes[0];   // INTEGER
  let decimal = u16bytes[1]; // DECIMAL
  
  return integer + (decimal / 100);
}

// Based on https://stackoverflow.com/a/37471538 by Ilya Bursov
function bytesToFloat(bytes) {
  var multiplied = u32(bytes);
  
  return multiplied / 100000 / 100;
}  

function u16(u8x2) {
  return (u8x2[1] << 8) | u8x2[0];
}

function u32(u8x4) {
  return (u8x4[3] << 24) | (u8x4[2] << 16) | (u8x4[1] << 8) | u8x4[0];
}

function decodeUplink(input) {
  var data = {};  
  data.event = "";
  if ((input.fPort & ACC_MASK) === ACC_MASK) {
    data.event += "A";
    data.accelAbs = u162sf([input.bytes[0], input.bytes[1]]);
  }
  if ((input.fPort & BAT_MASK) === BAT_MASK) {
    data.event += "B";
    data.vBat = u162sf([input.bytes[2], input.bytes[3]]);
  }
  if ((input.fPort & CO2_MASK) === CO2_MASK) {
    data.event += "C";
    data.co2 = u16([input.bytes[4], input.bytes[5]]);
    data.temperature = u162sf([input.bytes[6], input.bytes[7]]);
    data.humidity = u162sf([input.bytes[8], input.bytes[9]]);
  }
  if ((input.fPort & GPS_MASK) === GPS_MASK) {
    data.event += "G";
    data.fq = input.bytes[10];
    data.satellites = input.bytes[11];
    data.latitude = bytesToFloat([input.bytes[12], input.bytes[13], input.bytes[14], input.bytes[15]]);
    data.lat = String.fromCharCode(input.bytes[16]);
    data.longitude = bytesToFloat([input.bytes[17], input.bytes[18], input.bytes[19], input.bytes[20]]);
    data.lon = String.fromCharCode(input.bytes[21]);
  }

  var warnings = [];
  if (data.temperature < 15) {
    warnings.push("It's cold");
  }
  return {
    data: data,
    warnings: warnings
  };
}
/////////////////////////////// TTN CONVERTER

console.log(decodeUplink(input));