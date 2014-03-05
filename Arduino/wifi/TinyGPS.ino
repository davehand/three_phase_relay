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
  bool newdata = false;
  unsigned long start = millis();
  while (millis() - start < 8) {  // Update every 5 seconds
    if (feedgps()){
           newdata = true;
    }
  }
  if (newdata) {
    gpsdump(gps);
  }
}

// Get and process GPS data
void gpsdump(TinyGPS &gps) {
  float flat, flon;
  unsigned long age, date, time, fix_age;
  gps.f_get_position(&flat, &flon, &age);
  gps.get_datetime(&date, &time, &fix_age);
  
  int year;
  byte month, day, hour, minute, second, hundredths;

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &fix_age);
  Serial.print(flat, 4); Serial.print(", "); 
  Serial.println(flon, 4);
  Serial.print(hour); Serial.print(":"); Serial.print(minute);Serial.print(":");Serial.println(second);
 
}

// Feed data as it becomes available 
bool feedgps() {
  while (nss.available()) {
    if (gps.encode(nss.read()))
      return true;
  }
  return false;
}
