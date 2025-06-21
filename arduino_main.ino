
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <PID_v1.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;
Servo esc;
SoftwareSerial loraSerial(10, 11); // RX, TX

double altitude_input, motor_output, altitude_setpoint = 100.0;
double Kp = 2.0, Ki = 0.8, Kd = 1.5;
PID pid(&altitude_input, &motor_output, &altitude_setpoint, Kp, Ki, Kd, DIRECT);

File logFile;
const int chipSelect = 10;

enum FlightState { IDLE, TAKEOFF, CRUISE, LANDING };
FlightState state = IDLE;

void changeState(FlightState newState) {
  state = newState;
}

void setup() {
  Serial.begin(9600);
  loraSerial.begin(9600);
  bmp.begin();
  esc.attach(9);
  esc.writeMicroseconds(1000);
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(1100, 1900);
  SD.begin(chipSelect);
}

void handleTakeoff(double alt) {
  if (alt < 10) esc.writeMicroseconds(1400);
  else if (alt < 20) esc.writeMicroseconds(1600);
  else {
    esc.writeMicroseconds(1700);
    changeState(CRUISE);
  }
}

void handleLanding(double alt) {
  if (alt > 10) esc.writeMicroseconds(1500);
  else if (alt > 2) esc.writeMicroseconds(1300);
  else {
    esc.writeMicroseconds(1000);
    changeState(IDLE);
  }
}

void loop() {
  altitude_input = bmp.readAltitude();
  pid.Compute();
  esc.writeMicroseconds(motor_output);

  // Log
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print(millis());
    logFile.print(",");
    logFile.print(altitude_input);
    logFile.println();
    logFile.close();
  }

  // Telemetry
  loraSerial.print("$TEL,ALT:");
  loraSerial.print(altitude_input);
  loraSerial.println(",BATT:92%");

  if (loraSerial.available()) {
    String cmd = loraSerial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("$CMD")) {
      if (cmd.indexOf("TAKEOFF") != -1) changeState(TAKEOFF);
      else if (cmd.indexOf("LAND") != -1) changeState(LANDING);
      else if (cmd.indexOf("SETALT") != -1) {
        int index = cmd.indexOf(":");
        altitude_setpoint = cmd.substring(index+1).toFloat();
      }
    }
  }

  if (state == TAKEOFF) handleTakeoff(altitude_input);
  else if (state == LANDING) handleLanding(altitude_input);

  delay(1000);
}
