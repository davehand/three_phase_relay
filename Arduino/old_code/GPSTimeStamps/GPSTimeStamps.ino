

#include <SoftwareSerial.h>
#include <TinyGPS.h>                // Special version for 1.0

TinyGPS gps;
SoftwareSerial nss(6, 255);            // Yellow wire to pin 6

void setup() {
  Serial.begin(115200);
  nss.begin(4800);
  Serial.println("Reading GPS");
}

void loop() {

  while (nss.available()) {
    if (gps.encode(nss.read()))
      gpsdump(gps);
  }
}

// Get and process GPS data
void gpsdump(TinyGPS &gps) {
 
  unsigned long age, date, time, fix_age;
  gps.get_datetime(&date, &time, &fix_age);

  int year;
  byte month, day, hour, minute, second, hundredths, ms, ns, us;

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &ms, &ns, &us, &fix_age);

  char buffer[32];
  
  sprintf(buffer, "%02d:%02d:%02d:%02d:%02d:%02d:%02d", hour, minute, second, hundredths, ms, ns, us);

  Serial.println(buffer);

}


