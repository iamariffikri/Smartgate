
var RaspiCam = require("raspicam");
var serialport = require('serialport');

var output = "/home/pi/smart_gate/plate-"+new Date().getTime()+".jpg";
var camera = new RaspiCam({ 
	mode: "photo", 
	output: output,
	width: 2592,
	height: 1944,
	quality: "100",
	encoding: "jpg",
	timeout: 3000
});

serialport.list(function (err, ports) {
  ports.forEach(function(port) {
    console.log(port.comName);
  });
});

var SerialPort = serialport.SerialPort;
var portName = process.argv[2];
var myPort = new SerialPort(portName, {

	baudRate: 9600,
	parser: serialport.parsers.readline("\r\n")
});

myPort.on("open", onOpen);
myPort.on("data", onData);

function onOpen() {
	console.log("Open connection");
}

function onData(data) {
	console.log("On Data: "+data);

	if(data == "action:takePicture")
		takePicture();
}

function sendToSerial(data) {
	console.log("sending to serial: " + data);

	myPort.write(data);
}

function takePicture() {

	camera.start();
	camera.on("read", function(err, timestamp, filename){
    	console.log("saved");
    	// todo // process image and send plate number
	});	
}

function sendPlateNumber(plateNumber) {
	sendToSerial(plateNumber);
}

function processImage() {

}
