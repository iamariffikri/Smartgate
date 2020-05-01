# SmartGate
SmartGate, an IOT project using [OpenALPR](http://www.openalpr.com) Plate Recognition and LoRa in
[Jomhack 2016, Cyberview](http://www.cyberjayamalaysia.com.my/happenings/2016/08/05/events/jomhack-smart-cities-with-lora).

## Architecture

![alt text](Architecture.png "Architecture")

## Demo

https://youtu.be/xnhBhMNJLAk

## Hardware
* [The Things Uno](https://shop.thethingsnetwork.com/index.php/product/the-things-uno/)
* Light Sensor (or any other sensor; to detect incoming car)
* Servo Motor
* LCD 16x2
* LED red & green
* Rotaty Switch
* Raspberry Pi 3
* Camera module
* 16GB SD card

## Installation

### Board Setup
![alt text](BoardSetup.jpeg "Board Setup")

### Arduino

1. Upload [the code](https://github.com/iamariffikri/Smartgate/blob/master/arduino/Smartgate.ino)
2. Update the `devAddr`, `nwkSKey` & `appSKey` gotten from TTN dashboard

### Raspberry Pi

1. [Install OpenALPR](https://gist.github.com/amstanley/9da7febc9a3e3c2228ee)
2. [Install NodeJS & NPM ](http://raspberrypi.stackexchange.com/a/37976)
3. Upload [the code](https://github.com/iamariffikri/Smartgate/blob/master/rpi) and run `npm install && node run.js`

### Backend

1. Copy [this code](https://github.com/iamariffikri/Smartgate/blob/master/backend) to a server or laptop
2.  Update the `appEUI`, `accessKey` & `devEUI` and run `npm install && node ttn.js`

## License

Licensed under the [MIT license](http://opensource.org/licenses/MIT)
