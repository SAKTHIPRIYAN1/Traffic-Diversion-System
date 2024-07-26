#define BLYNK_TEMPLATE_ID "TMPL3U-0b9tNR"
#define BLYNK_TEMPLATE_NAME "Nm"
#define BLYNK_AUTH_TOKEN "0nByTdjbu8B7zddQUxvXs0XTmL7ivcuG"

#define BLYNK_PRINT Serial

#include <TM1637Display.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#define LANE0_AMB 42
#define LANE1_AMB 40
#define LANE2_AMB 41


// Lane 1 pin definitions
#define LANE0_GREEN_PIN 11
#define LANE0_RED_PIN 10
#define LANE0_CLK 3
#define LANE0_DIO 46

#define L0U1_Trig 9
#define L0U1_Echo 14

// this is for lane 0 cross walk...
#define L0U2_Trig 12
#define L0U2_Echo 13

// lane 0 cross signal leds..
#define LANE0_CROSS_GREEN 19
#define LANE0_CROSS_RED 20

// Lane 2 pin definitions
#define LANE1_GREEN_PIN 7
#define LANE1_RED_PIN 15
#define LANE1_CLK 18
#define LANE1_DIO 17

#define L1U1_Trig 6
#define L1U1_Echo 35

// this for lane 1 cross walk....
#define L1U2_Trig 5
#define L1U2_Echo 4


// lane 1 cross signal leds..
#define LANE1_CROSS_GREEN 8
#define LANE1_CROSS_RED 16


// Lane 3 pin definitions
#define LANE2_GREEN_PIN 47
#define LANE2_RED_PIN 21
#define LANE2_CLK 0
#define LANE2_DIO 45

// lane 2 cross signal leds..
#define LANE2_CROSS_GREEN 1
#define LANE2_CROSS_RED 2

#define L2U1_Trig 36
#define L2U1_Echo 37

// for lane 2 cross walk...
#define L2U2_Trig 38
#define L2U2_Echo 39



int greenTime[3] = {20, 20, 30};
int currentLane = 0;
int displayTime[3] = {greenTime[0], greenTime[0], greenTime[0] + greenTime[1]};

TM1637Display display0(LANE0_CLK, LANE0_DIO);
TM1637Display display1(LANE1_CLK, LANE1_DIO);
TM1637Display display2(LANE2_CLK, LANE2_DIO);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

void setup() {
  pinMode(LANE0_GREEN_PIN, OUTPUT);
  pinMode(LANE0_RED_PIN, OUTPUT);
  pinMode(LANE1_GREEN_PIN, OUTPUT);
  pinMode(LANE1_RED_PIN, OUTPUT);
  pinMode(LANE2_GREEN_PIN, OUTPUT);
  pinMode(LANE2_RED_PIN, OUTPUT);

  // crosswalk pins...
  pinMode(LANE0_CROSS_GREEN, OUTPUT);
  pinMode(LANE0_CROSS_RED, OUTPUT);
  pinMode(LANE1_CROSS_GREEN, OUTPUT);
  pinMode(LANE1_CROSS_RED, OUTPUT);
  pinMode(LANE2_CROSS_GREEN, OUTPUT);
  pinMode(LANE2_CROSS_RED, OUTPUT);

  pinMode(L0U1_Trig, OUTPUT);
  pinMode(L0U1_Echo, INPUT);
  pinMode(L0U2_Trig, OUTPUT);
  pinMode(L0U2_Echo, INPUT);

  pinMode(L1U1_Trig, OUTPUT);
  pinMode(L1U1_Echo, INPUT);
  pinMode(L1U2_Trig, OUTPUT);
  pinMode(L1U2_Echo, INPUT);

  pinMode(L2U1_Trig, OUTPUT);
  pinMode(L2U1_Echo, INPUT);
  pinMode(L2U2_Trig, OUTPUT);
  pinMode(L2U2_Echo, INPUT);

  pinMode(LANE0_AMB, OUTPUT);
  pinMode(LANE2_AMB, OUTPUT);
  pinMode(LANE1_AMB, OUTPUT);
  digitalWrite(LANE0_AMB, LOW);
   digitalWrite(LANE1_AMB, LOW);
  digitalWrite(LANE2_AMB, LOW);

  digitalWrite(LANE0_GREEN_PIN, LOW);
  digitalWrite(LANE0_RED_PIN, LOW);
  digitalWrite(LANE1_GREEN_PIN, LOW);
  digitalWrite(LANE1_RED_PIN, LOW);
  digitalWrite(LANE2_GREEN_PIN, LOW);
  digitalWrite(LANE2_RED_PIN, LOW);

  // initial cross setupp....
  digitalWrite(LANE0_CROSS_GREEN, LOW);
  digitalWrite(LANE0_CROSS_RED, HIGH);
  digitalWrite(LANE1_CROSS_GREEN, LOW);
  digitalWrite(LANE1_CROSS_RED, HIGH);
  digitalWrite(LANE2_CROSS_GREEN, LOW);
  digitalWrite(LANE2_CROSS_RED, HIGH);


  display0.setBrightness(0x0f);
  display1.setBrightness(0x0f);
  display2.setBrightness(0x0f);

  Serial.begin(115200);

  // Blynk setup
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  setLights(currentLane);

  while (displayTime[currentLane]) {
    Blynk.run();
    Main_control_Check();
    cross_check(currentLane);
    timecheck(currentLane);
    countDown();
  }
  

  currentLane = (currentLane + 1) % 3;
  displayTime[currentLane] = greenTime[currentLane];
  displayTime[(currentLane + 2) % 3] = greenTime[currentLane] + greenTime[(currentLane + 1) % 3];
}


void Main_control_Check() {
  if (digitalRead(LANE0_AMB) == HIGH) {
    display0.showNumberDec(007, true);
    display1.showNumberDec(007, true);
    display2.showNumberDec(007, true);

    off_cross(0);off_cross(1);off_cross(2);

    // delay(1500); // Short delay
    while (digitalRead(LANE0_AMB) == HIGH) {
      Blynk.virtualWrite(V3,"Lane 0");

      digitalWrite(LANE0_GREEN_PIN, HIGH);
      digitalWrite(LANE0_RED_PIN, LOW);
      digitalWrite(LANE1_GREEN_PIN, LOW);
      digitalWrite(LANE1_RED_PIN, HIGH);
      digitalWrite(LANE2_GREEN_PIN, LOW);
      digitalWrite(LANE2_RED_PIN, HIGH);
      Blynk.run(); // Keep Blynk connection alive
    }
    setLights(currentLane);
  } else if (digitalRead(LANE2_AMB) == HIGH) {
    off_cross(0);off_cross(1);off_cross(2);

    display0.showNumberDec(007, true);
    display1.showNumberDec(007, true);
    display2.showNumberDec(007, true);

    // delay(1500); // Short delay
    while (digitalRead(LANE2_AMB) == HIGH) {

      Blynk.virtualWrite(V3,"Lane 2");

      digitalWrite(LANE0_GREEN_PIN, LOW);
      digitalWrite(LANE0_RED_PIN, HIGH);
      digitalWrite(LANE1_GREEN_PIN, LOW);
      digitalWrite(LANE1_RED_PIN, HIGH);
      digitalWrite(LANE2_GREEN_PIN, HIGH);
      digitalWrite(LANE2_RED_PIN, LOW);
      Blynk.run(); // Keep Blynk connection alive
    }
    setLights(currentLane);

  } else if (digitalRead(LANE1_AMB) == HIGH) {
    off_cross(0);off_cross(1);off_cross(2);

    display0.showNumberDec(007, true);
    display1.showNumberDec(007, true);
    display2.showNumberDec(007, true);

    // delay(1500); // Short delay
    while (digitalRead(LANE1_AMB) == HIGH) {

      Blynk.virtualWrite(V3,"Lane 1");

      digitalWrite(LANE0_GREEN_PIN, LOW);
      digitalWrite(LANE0_RED_PIN, HIGH);
      digitalWrite(LANE2_GREEN_PIN, LOW);
      digitalWrite(LANE2_RED_PIN, HIGH);
      digitalWrite(LANE1_GREEN_PIN, HIGH);
      digitalWrite(LANE1_RED_PIN, LOW);
      Blynk.run(); // Keep Blynk connection alive
    }
    setLights(currentLane);
  }
}


void setLights(int currentLane) {
  // Serial.println(currentLane);
  
  if (currentLane == 0) {

    Serial.print("\nSwitched to lane0");

    Blynk.virtualWrite(V3,"Lane 0");

    digitalWrite(LANE0_GREEN_PIN, HIGH);
    digitalWrite(LANE0_RED_PIN, LOW);
    digitalWrite(LANE1_GREEN_PIN, LOW);
    digitalWrite(LANE1_RED_PIN, HIGH);
    digitalWrite(LANE2_GREEN_PIN, LOW);
    digitalWrite(LANE2_RED_PIN, HIGH);

    // checkforcross(currentLane);

  } else if (currentLane == 1) {

     Serial.print("\nSwitched to lane1");

    Blynk.virtualWrite(V3,"Lane 1");

    digitalWrite(LANE0_GREEN_PIN, LOW);
    digitalWrite(LANE0_RED_PIN, HIGH);
    digitalWrite(LANE1_GREEN_PIN, HIGH);
    digitalWrite(LANE1_RED_PIN, LOW);
    digitalWrite(LANE2_GREEN_PIN, LOW);
    digitalWrite(LANE2_RED_PIN, HIGH);

    // checkforcross(currentLane);

  } else if (currentLane == 2) {

     Serial.print("\nSwitched to lane2");

     Blynk.virtualWrite(V3,"Lane 2");

    digitalWrite(LANE0_GREEN_PIN, LOW);
    digitalWrite(LANE0_RED_PIN, HIGH);
    digitalWrite(LANE1_GREEN_PIN, LOW);
    digitalWrite(LANE1_RED_PIN, HIGH);
    digitalWrite(LANE2_GREEN_PIN, HIGH);
    digitalWrite(LANE2_RED_PIN, LOW);

    // checkforcross(currentLane);

  }
}

// cross walk..... function.....

void cross_check(int curr){
  
  // predestrians count...
  long predes0 = ultrasonic_dist(0, 2);
  long predes1 = ultrasonic_dist(1, 2);
  long predes2 = ultrasonic_dist(2, 2);

  if(predes0<20){
    change_cross(0,curr);
  }
  else{
    off_cross(0);
  }

  if(predes1<20){
    change_cross(1,curr);
  }
  else{
      off_cross(1);
  }

  if(predes2<20){
    change_cross(2,curr);
  }
  else{
    off_cross(2);
  
}
}

// if req not equal to the curr lane 
//  ==> allow predestrianss..


void change_cross(int req,int curr){
  if(req!=curr){
      if(req==0){
          digitalWrite(LANE0_CROSS_GREEN, HIGH);
          digitalWrite(LANE0_CROSS_RED, LOW);
      }
      else if(req==1){
          digitalWrite(LANE1_CROSS_GREEN, HIGH);
          digitalWrite(LANE1_CROSS_RED, LOW);
      }
      else if(req==2){
          digitalWrite(LANE2_CROSS_GREEN, HIGH);
          digitalWrite(LANE2_CROSS_RED, LOW);
      }
  }
}

void off_cross(int req){
      if(req==0){
          digitalWrite(LANE0_CROSS_GREEN, LOW);
          digitalWrite(LANE0_CROSS_RED, HIGH);
      }
      else if(req==1){
          digitalWrite(LANE1_CROSS_GREEN, LOW);
          digitalWrite(LANE1_CROSS_RED, HIGH);
      }
      else if(req==2){
          digitalWrite(LANE2_CROSS_GREEN, LOW);
          digitalWrite(LANE2_CROSS_RED, HIGH);
      }
}

//  it check if there is conjection in current lane ..
// if current lane congestion is less than switch to the next lane....

void timecheck(int current) {
  int nextLane = (current + 1) % 3;
  int nextnextLane = (current + 2) % 3;
  long next1 = ultrasonic_dist(nextLane, 1);
  // long next2 = ultrasonic_dist(nextLane, 2);
  long nextnext1 = ultrasonic_dist(nextnextLane, 1);
  // long nextnext2 = ultrasonic_dist(nextnextLane, 2);
  long current2 = ultrasonic_dist(current, 1);

  if (next1 < 100 && current2 > 100) {
    if (displayTime[currentLane] > 10) {
      int temp = displayTime[currentLane] - 10;
      for (int i = 0; i < 3; i++) {
        displayTime[i] -= temp;
      }

      display0.showNumberDec(8888, true);
      display1.showNumberDec(8888, true);
      display2.showNumberDec(8888, true);
    }
  }
}

void countDown() {
  --displayTime[0];
  --displayTime[1];
  --displayTime[2];

  display0.showNumberDec(displayTime[0], true);
  display1.showNumberDec(displayTime[1], true);
  display2.showNumberDec(displayTime[2], true);
  delay(100);
}

long readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;

  return distance;
}



long ultrasonic_dist(int track, int sensor) {
  long distance;
  if (track == 0) {
    if (sensor == 1) {
      distance = readUltrasonic(L0U1_Trig, L0U1_Echo);
    } else if (sensor == 2) {
      distance = readUltrasonic(L0U2_Trig, L0U2_Echo);
    }
  } else if (track == 1) {
    if (sensor == 1) {
      distance = readUltrasonic(L1U1_Trig, L1U1_Echo);
    } else if (sensor == 2) {
      distance = readUltrasonic(L1U2_Trig, L1U2_Echo);
    }
  } else if (track == 2) {
    if (sensor == 1) {
      distance = readUltrasonic(L2U1_Trig, L2U1_Echo);
    } else if (sensor == 2) {
      distance = readUltrasonic(L2U2_Trig, L2U2_Echo);
    }
  }
  return distance;
}

// Blynk write functions for lane 0 and lane 2 control state
BLYNK_WRITE(V0) {
  int lane0cont = param.asInt();
  Serial.print(lane0cont);
  digitalWrite(LANE0_AMB, lane0cont ? HIGH : LOW);
}

BLYNK_WRITE(V1) {
  int lane1cont = param.asInt();
  digitalWrite(LANE1_AMB, lane1cont ? HIGH : LOW);
}

BLYNK_WRITE(V2) {
  int lane2cont = param.asInt();
  digitalWrite(LANE2_AMB, lane2cont ? HIGH : LOW);
}
