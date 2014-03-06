var app = require('http').createServer(handler)
  , io = require('socket.io').listen(app)
  , fs = require('fs')

app.listen(8070);

var serialPort = '/dev/tty.usbmodem1421';
var arduino = require('./firmataConnector').start(serialPort);

var prestate = true;
var crosstime = 0;
var pretime = 0;
var phasetime = 0;

function handler (req, res) {
    fs.readFile(__dirname + '/index.html',
    function (err, data) {
      if (err) {
        res.writeHead(500);
        return res.end('Error loading index.html');
      }

      res.writeHead(200);
      res.end(data);
    });
}

io.sockets.on('connection', function (socket) {

  socket.on('message', function (msg) {
    console.log(msg);
  })

  socket.on('disconnect', function () {
    console.log('client disconnected: ' + socket.id);
  })
});

arduino.on('connection', function () {
  console.log("successfully connected to Arduino!");

  arduino.analogRead(arduino.A0, function(val) {
  console.log(val);
 });
  
  arduino.pinMode(6, arduino.OUTPUT);
  arduino.pinMode(8, arduino.INPUT);

  arduino.digitalRead(8, function(val){
    console.log(val);
  });
});
