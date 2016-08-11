

var RaspiCam = require("raspicam");
var fs = require("fs");
var request = require("request");
var serialport = require('serialport');

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
	camera.start();
	camera.on("read", function(err, timestamp, filename){
    		console.log("saved");
    		// todo // process image and send plate number
		if(filename.indexOf('~') < 0)
			checkALPR(output);
	});
}

function sendPlateNumber(plateNumber) {
	sendToSerial(plateNumber);
}

function processImage() {


}

function checkALPR(imagePath) {
        console.log('alpr -c sg ' + imagePath);
	const exec = require('child_process').exec;
	const child = exec('alpr -c sg ' + imagePath, (error, stdout, stderr) => {

	    var str = `${stdout}`;
	    var arr = str.split('\n');

	    if (arr.length > 9) {
	        // success
	        var goodResult = arr[1];
	        goodResult = goodResult.trim();
	        goodResult = goodResult.replace('-', '');
	        goodResult = goodResult.replace(' ', '');
	        goodResult = goodResult.replace('\t', '');
	        var values = goodResult.split('confidence:')
	        console.log(values);
	        console.log('plate found');
		sendPlateNumber(values[0]);

		return values[0]; // this is the plate number
	    }
	    else {
	        // not found
	        console.log('not recognized');
		sendToSerial('notRecognized');
                return false;
	    }

	/*
	    console.log(`stdout: ${stdout}`);
	    console.log(`stderr: ${stderr}`);
	    console.log(`error: ${error}`);
	*/
});
}

function processImageByAPI() {

	var url = "https://api.openalpr.com/v1/recognize?secret_key=sk_a60c5999e29f6d9664b7ed62&tasks=plate&return_image=0&country=SG";
	var req = request.post(url, function (err, resp, body) {
		if (err) {
			console.log('Error!');
		} else {
			if(typeof body.plate.results !== "undefined" && body.plate.results.length > 0)
			sendPlateNumber(body.plate.results[0].plate);
		}
	});
	var form = req.form();
	form.append('image', fs.createReadStream(
));
}
