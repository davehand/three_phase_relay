#include <Base64.h>
#include <global.h>
#include <MD5.h>
#include <sha1.h>
#include <WebSocketClient.h>
#include <WebSocketServer.h>

#include <SPI.h>
#include <WiFi.h>

char ssid[] = "SteveAndTimECE";      //  your network SSID (name)
char pass[] = "DeesePowerSystems";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int voltageReading = 0;
   
int relay=6;
int cross=8;
int peakpin=A0;
int peak;
int pretime=0;
int phasetime;
int crosstime;
bool state;
bool prestate=0;
String HTTP_req;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);      // initialize serial communication
  pinMode(9, OUTPUT);      // set the LED pin mode
  //pinMode(A0, INPUT);
  
  pinMode(peakpin,INPUT);
  pinMode(relay,OUTPUT);
  pinMode(cross,INPUT);
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {

  peak=analogRead(peakpin);
  state=digitalRead(cross);
  if(state!=prestate){
    crosstime=micros();
    phasetime=crosstime-pretime;
    }
   digitalWrite(relay,!state);

  prestate=state;
  pretime=crosstime;
  Serial.println(phasetime);
  
            
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();        // read a byte, then
        HTTP_req +=c;
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line: 
                  client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();
                    // AJAX request for switch state
                    if (HTTP_req.indexOf("ajax_switch") > -1) {
                        // read switch state and send appropriate paragraph text
                        GetSwitchState(client);
                    }
                    else {  // HTTP request for web page
                        // send web page - contains JavaScript with AJAX calls
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                        client.println("<head>");
                        client.println("<title>Arduino Web Page</title>");
                        client.println("<script>");
                        client.println("function GetSwitchState() {");
                        client.println("nocache = \"&nocache=\"\+ Math.random() * 1000000;");
                        client.println("var request = new XMLHttpRequest();");
                        client.println("request.onreadystatechange = function() {");
                        client.println("if (this.readyState == 4) {");
                        client.println("if (this.status == 200) {");
                        client.println("if (this.responseText != null) {");
                        client.println("document.getElementById(\"switch_txt\")\.innerHTML = this.responseText;");
                        client.println("}}}}");
                        client.println(
                        "request.open(\"GET\", \"ajax_switch\" + nocache, true);");
                        client.println("request.send(null);");
                        client.println("setTimeout('GetSwitchState()', 1000);");
                        client.println("}");
                        client.println("</script>");
                        client.println("</head>");
                        client.println("<body onload=\"GetSwitchState()\">");
                        client.println("<h1>Arduino AJAX Switch Status</h1>");
                        client.println(
                        "<p id=\"switch_txt\">Switch state: Not requested...</p>");
                        client.println("</body>");
                        client.println("</html>");
                    }

            
            //client.print("Click <a href=\"/H\">here</a> turn the LED on pin 9 on<br>");
            //client.print("Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(9, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(9, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

// send the state of the switch to the web browser
void GetSwitchState(WiFiClient cl)
{
    if (digitalRead(peakpin)) {
        cl.print("Peak Voltage Reading:");
        cl.println(peak);
    }
    else {
        cl.println("Switch state: OFF");
    }
    
    
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
