#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <SPI.h>

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

void setup()
{
  Serial.begin(115200);      // initialize serial communication
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
    //delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus(); 
}

void loop()
{
  
    WiFiClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
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
                        // send web page - contains JavaScript with AJAX 
                        
                         client.println("<!DOCTYPE html>");
  
                         client.println("<html lang=\"en\">");
                         client.println("<head>");
                         client.println("<meta charset=\"UTF-8\">");
                         client.println("<title>Default HTML</title>");
                         client.println("<script src=\"/../socket.io/socket.io.js\"></script>");
                         client.println("<script>");
                         client.println("function init() {");
                       //connect to the socket server
                         client.println("var socket = io.connect('http://127.0.0.1:8070');");
                        //on connect
                        client.println("socket.on('connect', function() {");
                        //send message
                        client.println("socket.send('hello from client');");
                        client.println("while(1) {");
                        client.println("socket.send(Math.random());");
                        client.println("}");
                        client.println("});");
                        client.println("}");
                        client.println("</script>");
                        client.println("</head>");
                        
                        client.println("<body> ");
                        client.println("<h1>Welcome!</h1>");
                        client.println("<p>It's running</p>");
                        client.println("<button onclick=\"init()\">Send Data</button>");
                        client.println("</body>");
                        
                        client.println("</html>");
                       
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// send the state of the switch to the web browser
void GetSwitchState(WiFiClient cl)
{
       peak=analogRead(peakpin);
        state=digitalRead(cross);
        if(state!=prestate){
          crosstime=micros();
          phasetime=crosstime-pretime;
          }
       digitalWrite(relay,!state);
 
      prestate=state;
      pretime=crosstime;
      //Serial.println(phasetime);
    
      
      cl.println("Peak Value");
      cl.println(peak);
      cl.println("");
      cl.println("Phase Time");
      cl.println(phasetime);
  
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
