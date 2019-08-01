//////////////////////////////////////////////////////////////
// Contributors:                                            //
// https://www.instructables.com/id/Arduino-GPS-Logger/     //
// https://www.instructables.com/member/Big.Jack.Jazz/      //
//////////////////////////////////////////////////////////////

#include <SD.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);

Adafruit_GPS GPS(&mySerial);

String NMEA1;
String NMEA2;

char c;
float deg;
float degWhole;
float degDec;
const int chipSelect = 10;

File mySensorData;

void setup() {
  Serial.begin(115200);
  GPS.begin(9600);

  GPS.sendCommand("$PGCMD,33,0*6D");
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  delay(1000);

  SD.begin(chipSelect);

  readGPS();

  // Initialize standard NMEA file
  if (SD.exists("NMEA.txt")) {
    mySensorData = SD.open("NMEA.txt", FILE_WRITE);
    mySensorData.println("<----------------------- New Session ----------------------->");
    mySensorData.print("***  ");
    mySensorData.print(GPS.month);
    mySensorData.print(".");
    mySensorData.print(GPS.day);
    mySensorData.print(".");
    mySensorData.print(GPS.year);
    mySensorData.print(" -- ");
    mySensorData.print(GPS.hour);
    mySensorData.print(":");
    mySensorData.print(GPS.minute);
    mySensorData.print(":");
    mySensorData.print(GPS.seconds);
    mySensorData.println("  ***");
    mySensorData.close();
  }

  // Initialize raw data file
  if (SD.exists("GPSData.txt")) {
    mySensorData = SD.open("GPSData.txt", FILE_WRITE);
    mySensorData.println("");
    mySensorData.println("<----------------------- New Session ----------------------->");
    mySensorData.print("***  ");
    mySensorData.print(GPS.day);
    mySensorData.print(".");
    mySensorData.print(GPS.month);
    mySensorData.print(".");
    mySensorData.print(GPS.year);
    mySensorData.print(" -- ");
    mySensorData.print(GPS.hour);
    mySensorData.print(":");
    mySensorData.print(GPS.minute);
    mySensorData.print(":");
    mySensorData.print(GPS.seconds);
    mySensorData.println("  ***");
    mySensorData.close();
  }
} // setup

void loop() {
  readGPS();

  if (GPS.fix == 1) { // If fix on satellite

    // Write to NMEA.txt
    mySensorData = SD.open("NMEA.txt", FILE_WRITE);
    
    mySensorData.println(NMEA1);
    mySensorData.println(NMEA2);
    mySensorData.close();

    // Write to GPSData.txt
    mySensorData = SD.open("GPSData.txt", FILE_WRITE);
    
    convLong();
    mySensorData.print(deg, 4);
    mySensorData.print(",");
    Serial.print(deg);
    Serial.print(",");

    convLati();
    mySensorData.print(deg, 4);
    mySensorData.print(",");
    Serial.print(deg);
    Serial.print(",");

    mySensorData.print(GPS.altitude);
    mySensorData.print("\n");
    Serial.println(GPS.altitude);
    mySensorData.close();
  }
} // loop

void readGPS() {
  clearGPS();
  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }

  GPS.parse(GPS.lastNMEA());
  NMEA1 = GPS.lastNMEA();

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  NMEA2 = GPS.lastNMEA();

  Serial.println(NMEA1);
  Serial.println(NMEA2);
  // Serial.println("");
} // readGPS


void clearGPS() {
  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

} // clearGPS

void convLong() { // Get logitude from deg
  degWhole = float(int(GPS.longitude / 100));
  degDec = (GPS.longitude - degWhole * 100 ) / 60;
  deg = degWhole + degDec;
  if (GPS.lon == 'W') {
    deg = (-1) * deg;
  }
} // convLong

void convLati() { // Get latitude from deg
  degWhole = float(int(GPS.latitude / 100));
  degDec = (GPS.latitude - degWhole * 100 ) / 60;
  deg = degWhole + degDec;
  if (GPS.lat == 'S') {
    deg = (-1) * deg;
  }
} // convLati
