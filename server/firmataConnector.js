"use strict";

var firmata = require('firmata');

/**
 * firmataConnector.js
 * 
 * ABOUT
 * Connect to the Arduino via the Firmata protocol.
 *
 */

// ANSI escape codes (used to color the server logs)
var magenta = '\u001b[35m';
var green    = '\u001b[32m';
var red      = '\u001b[31m';
var reset    = '\u001b[0m';


exports.start = function (serialPort) {

    if(! serialPort) {
        
        console.error('no port address set for the arduino');
        return;
    }
    
    // connect to the Arduino
    var board = new firmata.Board (serialPort, function (err) {
        
        if (err) {
            console.log(red + err + reset);
            return;
        }
        
        //arduino is ready to communicate
        console.log(green +'Successfully connected to your Arduino'+ reset);
        console.log('Firmware: '+ board.firmware.name +'-'+
                                  board.firmware.version.major +'.'+
                                  board.firmware.version.minor);
        
        // set pin mode short hands
        board.INPUT  = board.MODES.INPUT;
        board.OUTPUT = board.MODES.OUTPUT;
        board.ANALOG = board.MODES.ANALOG;
        board.PWM    = board.MODES.PWM;
        board.SERVO  = board.MODES.SERVO;
        
        // analog pins
        board.A0 = board.pins[board.analogPins[0]].analogChannel;
        board.A1 = board.pins[board.analogPins[1]].analogChannel;
        board.A2 = board.pins[board.analogPins[2]].analogChannel;
        board.A3 = board.pins[board.analogPins[3]].analogChannel;
        board.A4 = board.pins[board.analogPins[4]].analogChannel;
        board.A5 = board.pins[board.analogPins[5]].analogChannel;
        
        // sensor-shield ports (untested)
        board.I0 = board.A0;
        board.I1 = board.A1;
        board.I2 = board.A2;
        board.I3 = board.A3;
        board.I4 = board.A4;
        board.I5 = board.A5;
        
        board.O0 = 11;
        board.O1 = 10;
        board.O2 = 9;
        board.O3 = 6;
        board.O4 = 5;
        board.O5 = 3;
        
        board.emit('connection');
    });
    
    board.lineLog = function (msg) {
        
        process.stdout.clearLine();  // clear previous output
        process.stdout.cursorTo(0);  // move cursor to beginning of line
        process.stdout.write(msg);
    };
    
    return board;
}


