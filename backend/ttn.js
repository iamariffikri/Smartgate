
var ttn = require('ttn');
var atob = require('atob');
 
var appEUI = '';
var accessKey = '';
var client = new ttn.Client('staging.thethingsnetwork.org', appEUI, accessKey);
var devEUI = '';
 
client.on('connect', function() {
  console.log('connected')
})
 
client.on('uplink', function (msg) {
  // These are messages sent by devices on The Things Network 
  // msg = { 
  //   devEUI: '00000000973572D0', 
  //   fields: { /* ... */ }, 
  //   counter: 44, 
  //   metadata: { /* ... */ }, 
  // } 
  if(typeof msg.fields.raw !== "undefined") {
    var raw_msg = atob(msg.fields.raw);
    if( raw_msg.length > 4) {
      // get plate number
      var plateNumber = raw_msg;//new Buffer(msg.fields.raw, 'base64').toString());
      console.log('Plate: '+raw_msg);
      // Send a downlink message to the device
      var payload = new Buffer('01', 'hex')
      client.downlink(devEUI, payload, '1h', 1)
    }
  } else
  if(typeof msg.fields !== "undefined")
    console.log(msg.fields);
})
 
client.on('activation', function (evt) {
  // These are devices that are activated on The Things Network 
  // evt = { 
  //   devEUI: '00000000973572D0', 
  // } 
})
 
client.on('error', function (err) {
  // TODO: Handle error 
  console.log(err);
})
 
// End the client connection 
//client.end()