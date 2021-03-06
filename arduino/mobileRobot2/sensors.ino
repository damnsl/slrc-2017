/*---------------------------------------------------------------------------
  Functions (public)

  void irSensorBegin()
  void colorSensorBegin()
  void sonarSensorBegin()

  int readIRSensors(*sensorValues)
  int readSonar(sensorNo)

  boolean isBoxFound()
  int readBoxColor()              return [COLOR_RED,COLOR_GREEN,COLOR_BLUE]

  Functions (private)

  int   irSensorRead
  ---------------------------------------------------------------------------*/

void irSensorBegin() {

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(irPins[i], INPUT);
  }
  Serial.println(F(">> IRSensors : Begin"));

}

void colorSensorBegin() {

  if (colorSensor.begin()) {
    // No error, can run program without problem
    Serial.println(F(">> ColorSensor : Begin"));
  } else {
    Serial.println(F(">> ColorSensor : Not Found"));
    beep(3);
  }
}

void sonarSensorBegin() {

  Serial.println(">> Sonar : Begin");

  for (int i = 0; i < NUM_SONAR; i++) {
    pinMode(pinTrig[i], OUTPUT);
    pinMode(pinEcho[i], INPUT);
    digitalWrite(pinTrig[i], LOW);
  }
}


//-------------------------------------------------------------------------------------------------------------- readIRSensor

int readIRSensors(unsigned int *sensor_values) {
  delay(10);

  weight = 0;
  sum = 0;
  allIn = false;
  allOut = false;
  irLineString = "";

  for (int i = 0; i < NUM_SENSORS; i++) {
    value = irSensorRead(i);
    sensor_values[i] = value;
    weight += value * (i * 10);
    sum += value;
    irLineString = irLineString + " " + (String)value;
  }

  if (sum == NUM_SENSORS) allIn = true;
  if (sum == 0) allOut = true;

  if (allOut)
  {
    if ((lastReading < RIGHT_EDGE_READING) && (CENTER_EDGE_READING + 10 > lastReading )) {        // <---***>
      lastReading = RIGHT_EDGE_READING;

    } else if ((LEFT_EDGE_READING < lastReading) && ( lastReading  > CENTER_EDGE_READING) - 10) { // <***--->
      lastReading = LEFT_EDGE_READING;

    } else if (lastReading == CENTER_EDGE_READING) {                                              // <--**-->
      lastReading = CENTER_EDGE_READING;
    }
  } else {
    lastReading = weight / sum;
  }

  //if (0)Serial.print(">> IR : "); Serial.println(irLineString);

//  long tt = millis();
  for (int x = 9; x > 0; x--) {
    for (int y = 0; y < 6; y++) {
      irHistory[x][y] = irHistory[x - 1][y];
    }
  }
  for (int y = 0; y < 6; y++) {
    irHistory[0][y] = sensor_values[y];
  }
//  Serial.print("T: "); Serial.println(millis() - tt);

  return lastReading;
}


//-------------------------------------------------------------------------------------------------------------- readBoxColor
int readBoxColor() {

  boxColor = 0;
  colorSensor.getRawData(&raw_red, &raw_green, &raw_blue, &raw_clr);

  if (raw_green > raw_blue && raw_green > raw_red ) {
    boxColor = COLOR_GREEN;               // GREEN
  }

  else if (raw_blue > raw_red ) {
    boxColor = COLOR_BLUE;                // BLUE
  }
  else if (((raw_green - raw_blue) < RED_GB_GAP) || ((raw_blue - raw_green) < RED_GB_GAP) ) {
    boxColor = COLOR_RED;                 // RED
  }
  else {
    boxColor = COLOR_OPEN;                // NO COLOR
  }

  /*
    if (boxColor == 1)Serial.println("R");
    else if (boxColor == 2)Serial.println("G");
    else if (boxColor == 3)Serial.println("B");
    else Serial.println("None");
  */
  if (0) {
    Serial.print(raw_blue);
    Serial.print(" ");
    Serial.print(raw_red);
    Serial.print(" ");
    Serial.print(raw_green);
    Serial.print(" ");
    Serial.print(raw_clr);
    Serial.print(" ");
    //Serial.print(colorSensor.calculateColorTemperature(raw_red, raw_green, raw_blue) );
    //Serial.print(" ");
  }
  delay(50);
  return boxColor;
}

//-------------------------------------------------------------------------------------------------------------- readBoz

boolean isBoxFound() {
  boxSensor = analogRead(PIN_BOX_SENSOR);
  boxFound = (boxSensor > BOX_FOUND_THERSOLD);
  //Serial.println(boxSsensor);
  return boxFound;
}

//-------------------------------------------------------------------------------------------------------------- readSonar
double readSonar(int sensor) {

  digitalWrite(pinTrig[sensor], LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig[sensor], HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig[sensor], LOW);

  duration = pulseIn(pinEcho[sensor], HIGH, 30000); //30000 : timeout period(us)

  if (duration == 0) {
    distance = 1000;
  } else {
    distance = duration / 58;
  }

  // #################### CHECK THESE VALUES ################
  if (sensor == 0 or sensor == 3) {
    distance -= 3;
  }

  /*distance = max(1, distance);
    distance = min(100, distance);*/
  /*if (distance > maxDistance) {
    distance = 100;
    }*/
  return distance;

}









//--- Private Functions -----------------------------------------------------------------------------------------------------------

int irSensorRead(int num) {
  int reading = digitalRead(irPins[num]);

  //reading = (reading > 512); //For analog mode
  if (lineType == BLACK) reading = 1 - reading;

  //reading = reading; //BLACK=0, WHITE=1
  return reading;
}
