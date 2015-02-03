#include <Time.h>
#include <TimeAlarms.h>
#include <SoftwareSerial.h>

const int BUZZER = 3;
const int BLUE = 9;
const int GREEN = 10;
const int RED = 11;  
const int PIN13 = 13;
const int OFF_BUTTON = 4;

int lookup[64] = {1,2,4,6,9,12,16,20,25,30,36,
42,49,56,64,72,81,90,100,110,121,132,
144,156,169,182,196,210,225,240,256,272,289,
306,324,342,361,380,400,420,441,462,484,506,
529,552,576,600,625,650,676,702,729,756,784,
812,841,870,900,930,961,992,992,992};

SoftwareSerial Genotronex(0, 1);

int alarmHour = 0;
int alarmMinute = 0;
int alarmSecond = 0;

int alarmSet = 0;
int alarmDuration = 255; //seconds

int offButtonState = 0;

long alarmSteps = 64;
long alarmDurationSeconds = 22;
AlarmId myTimer;

int lastHour = 0;
int lastMinute = 0;
int lastSecond = 0;

void setup(){
  buzz();
  
//  Genotronex.begin(9600);
//  Genotronex.println("Bluetooth On");
  Serial.begin(9600);
  Serial.println("serial on");
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(PIN13, OUTPUT);
  pinMode(OFF_BUTTON, INPUT);
 
  setTime(18,45,30,1,12,15); 
 // setArduinoAlarm(18,45,35);
}

void loop(){
  Alarm.delay(1000);
  if(alarmSet == 0){
    digitalWrite(PIN13,0);
  }
  else{
    digitalWrite(PIN13,1);
  }

  //while (Genotronex.available() > 0){
  while(Serial.available() > 0){  

  Alarm.delay(1000);
    int command = Serial.parseInt();

    switch (command){
      case 1:
        setArduinoTime(Serial.parseInt(),Serial.parseInt(),Serial.parseInt(),Serial.parseInt(),Serial.parseInt(),Serial.parseInt());
        Serial.println("Time Set");
        buzz();
        break;
      case 2:
        digitalClockDisplay();
        break;
      case 3:
        alarmHour = Serial.parseInt();
        alarmMinute = Serial.parseInt();
        alarmSecond = Serial.parseInt();
        setArduinoAlarm(alarmHour, alarmMinute, alarmSecond);
        Serial.print("Alarm Set to: ");
        Serial.print(alarmHour);
        Serial.print(", ");
        Serial.print(alarmMinute);
        Serial.print(", ");
        Serial.println(alarmSecond);
        break;
      case 4:
        getAlarm();
        break;
      case 5:
        enableAlarm();
        break;
      case 6:
        disableAlarm();
        break;
	  /** Debug Commands
		The following are commands not used in final code, but are helpful for debugging
	  */
      case 13:
		buzz();
		break;
      case 14:
		alarm();
	break;
      case 15:
        int LEDred = Serial.parseInt();
        int LEDgreen = Serial.parseInt();
        int LEDblue = Serial.parseInt();
        LEDs(LEDred, LEDgreen, LEDblue);
        break;
    }
  }
}

/*Serial Commands
*************************************************
*/
//command 1
void setArduinoTime(int hour, int minute, int second, int day, int month, int year) { 
  setTime(hour, minute, second, day, month, year);
  getTime();
}

//command 2
void getTime(){
  digitalClockDisplay();
}

//command 3
void setArduinoAlarm(int hour, int minute, int second){
  lastHour = hour;
  lastMinute = minute;
  lastSecond = second;
  
  Alarm.alarmRepeat(hour, minute, second, alarm);
  
  alarmSet = 1;
  
  Serial.println(hour);
  Serial.println(minute);
  Serial.println(second);  
}

/**
	command 4
	Prints the last alarm to serial
*/
void getAlarm(){
  Serial.print("Last Known Alarm:");
  Serial.print(alarmHour);
  Serial.print(", ");
  Serial.print(alarmMinute);
  Serial.print(", ");
  Serial.println(alarmSecond);
}

/**
	command 5
	Enables the arduino alarm
*/
void enableAlarm(){  
  Alarm.enable(myTimer);
  Serial.println("Alarm Enabled");
}

//command 6
void disableAlarm(){
  Alarm.disable(myTimer);
  Serial.println("Alarm Disabled");
}

//command 13
void buzz(){
  tone(BUZZER,440);
  delay(500);
  noTone(BUZZER);
  Serial.println("buzzed");
}

//cmd 14
void alarm(){
//this is the actual function that runs when the alarm goes off
  int redVal = 0;
  int blueVal = 0;
  int greenVal = 0;
  
  int delayValue = (alarmDurationSeconds*1000)/alarmSteps;
  Serial.println(alarmDurationSeconds*1000);
  Serial.println(alarmSteps);
  Serial.println(delayValue);
  for(int i = 0; i<64; i++){
    offButtonState = digitalRead(OFF_BUTTON);
    if(offButtonState == 0){
      buzz();
      break;
    }
    else{
      redVal = map(lookup[i], 0, 1023, 0, 255);
      LEDs(redVal,redVal,redVal);
      delay(delayValue);
    }
  }
  LEDs(redVal,0,0);
  Serial.println("alarmed");
  while(true){
    offButtonState = digitalRead(OFF_BUTTON);
    if(offButtonState == 0){
	  alarmSet = 0;
      break;
    }
    else{
      buzz();
      delay(1000);
    }
  }
}
/**
	Helper function to print the time to serial
*/
void digitalClockDisplay()
{
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 
}
/**
	Helper function to print digits 
*/
void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/**
	Helper function to write frequencies to LEDs
*/
void LEDs(int red, int green, int blue){
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}
