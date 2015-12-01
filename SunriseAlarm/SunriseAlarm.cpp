SunriseAlarm::SunriseAlarm()
{
  buzz();
  SoftwareSerial Genotronex(0, 1);
  Genotronex.begin(9600);
  Genotronex.println("Bluetooth On");
  Serial.begin(9600);
  Serial.println("serial on");
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(INDICATOR_LED, OUTPUT);
  pinMode(OFF_BUTTON, INPUT);
 
  setTime(12,0,0,1,1,15);
  buzz();  
}

void SunriseAlarm::dot()
{
}

int lookup[64] = {1,2,4,6,9,12,16,20,25,30,36,
42,49,56,64,72,81,90,100,110,121,132,
144,156,169,182,196,210,225,240,256,272,289,
306,324,342,361,380,400,420,441,462,484,506,
529,552,576,600,625,650,676,702,729,756,784,
812,841,870,900,930,961,992,992,992};

int lookupRed[64] = {0,0,0,0,0,0,1,2,4,6,9,12,16,20,25,30,36,42,49,56,64,72,81,90,100,110,121,132,144,156,169,182,196,210,225,240,256,272,289,316,324,342,361,380,400,420,441,462,484,506,529,552,576,600,784,812,841,870,900,930,961,992,992,992};

int lookupGreen[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,992,992,992};

int lookupBlue[64] = {1,2,4,6,9,12,16,20,25,30,36,42,49,56,64,72,81,90,100,110,121,132,144,156,169,182,196,210,225,240,256,272,289,316,324,342,361,380,400,420,441,462,484,506,529,552,576,600,600,600,600,550,500,450,400,350,300,300,300,300,300,300,300,300};

long alarmSteps = 64;
long alarmDurationSeconds = 22;

const int BUZZER = 12;
const int BLUE = 9;
const int GREEN = 10;
const int RED = 11;  
const int INDICATOR_LED = 13;
const int OFF_BUTTON = 4;

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
      redVal = map(lookupRed[i], 0, 1023, 0, 255);
	  greenVal = map(lookupGreen[i], 0, 1023, 0, 255);
	  blueVal = map(lookupBlue[i], 0, 1023, 0, 255);
	  LEDs(redVal,greenVal,blueVal);
      delay(delayValue);
    }
  }
  LEDs(redVal,greenVal,blueVal);
  Serial.println("alarmed");
  for(int i = 0; i<5; i++){
    offButtonState = digitalRead(OFF_BUTTON);
    if(offButtonState == 0){
	  alarmSet = 0;
	  LEDs(0,0,0);
      break;
    }
    else{
      buzz();
      delay(1000);
    }
  }
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

//command 13
void buzz(){
  tone(BUZZER,440);
  delay(500);
  noTone(BUZZER);
  Serial.println("buzzed");
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
	command 4
	Prints the last alarm to serial
*/
void getAlarm(){
  Serial.println("Last Known Alarm:");
  Serial.print(alarmHour);
  Serial.print(":");
  Serial.print(alarmMinute);
  Serial.print(":");
  Serial.println(alarmSecond);
}

//command 2
void getTime(){
  digitalClockDisplay();
}

//command 3
void setArduinoAlarm(int hour, int minute, int second){
  alarmHour = hour;
  alarmMinute = minute;
  alarmSecond = second;
  
  Alarm.alarmRepeat(hour, minute, second, alarm);
  
  alarmSet = 1;
  
  Serial.println("Alarm Set to:");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);  
}


void initialize(){
  buzz();
  LEDs(255,0,0);
  delay(1000);
  LEDs(0,255,0);
  delay(1000);
  LEDs(0,0,255);
  delay(1000);
  LEDs(255,255,255);
  delay(1000);
  LEDs(0,0,0);
}


/*Serial Commands
*************************************************
*/
//command 1
void setArduinoTime(int hour, int minute, int second, int day, int month, int year) { 
  setTime(hour, minute, second, day, month, year);
  getTime();
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