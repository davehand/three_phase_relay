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
int peak_max=0;
int phase_sum=0;
int phase_avg = 0;
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
    
    createWebPage(client);

    Serial.println(phase_avg);

    getData();
}

// send the state of the switch to the web browser
void getData()
{
     //set up to run for 1 second
     int startTime = millis();
     int endTime = startTime;
     //resetting values
     int i = 1;
     peak_max = 0;
     pretime = micros();
     phase_sum = 0;
     phase_avg = 0;

     while ((endTime - startTime) <= 1000) {

       //read peak and phasetime
       peak=analogRead(peakpin);
       state=digitalRead(cross);
       if(state!=prestate){
          crosstime=micros();
          phasetime=crosstime-pretime;
       }
       //steve's thing
       digitalWrite(relay,!state);

       //sets values for next iteration
       prestate=state;
       pretime=crosstime;

       //find max peak in second
       if (peak > peak_max) {
         peak_max = peak;
       }

       //only do a few readings for phase b/c Arduino ints are small
       if (i < 2) {
         
         //calculate phase time value
         if (phasetime > 5000) {
           phase_avg = phasetime;
           i++;
         }
       }
      
       endTime = millis();
    }
}

void createWebPage (WiFiClient client) {

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
                        // send web page - contains JavaScript with AJAX calls
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                        client.println("<head>");
                        client.println("<title>Arduino Web Page</title>");
                        client.println("<script>");
                        client.println("function GetSwitchState() {");
                        client.println("nocache = \"&nocache=\"+ Math.random() * 1000000;");
                        client.println("var request = new XMLHttpRequest();");
                        client.println("request.onreadystatechange = function() {");
                        client.println("if (this.readyState == 4) {");
                        client.println("if (this.status == 200) {");
                        client.println("if (this.responseText != null) {");
                        client.println("document.getElementById(\"switch_txt\").innerHTML = this.responseText;");
                        client.println("}}}}");
                        client.println("request.open(\"GET\", \"ajax_switch\" + nocache, true);");
                        client.println("request.send(null);");
                        client.println("setTimeout('GetSwitchState()', 1000);");
                        client.println("}");
                        client.println("</script>");
                        client.println("</head>");
                        client.println("<body onload=\"GetSwitchState()\">");
                        client.println("<h1>Smart Three Phase Power System Status</h1>");
                        client.println("<p id=\"switch_txt\">Loading data...</p>");
                        client.println("<a href=\"http://tcnjsmart.weebly.com\">TCNJ SMART Home Page</a>");
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

void GetSwitchState(WiFiClient cl){

  cl.println("Peak: ");
  cl.println(peak_max);
  cl.println(" V");
  cl.println("Phase: ");
  cl.println(phase_avg);
  cl.println(" ms");
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
