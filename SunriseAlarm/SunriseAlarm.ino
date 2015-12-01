#include <Time.h>
#include <TimeAlarms.h>
#include <SoftwareSerial.h>

int alarmHour = 0;
int alarmMinute = 0;
int alarmSecond = 0;

int alarmSet = 0;

int offButtonState = 0;

AlarmId myTimer;

void setup(){  
}

void loop(){
  Alarm.delay(1000);
  if(alarmSet == 0){
    digitalWrite(INDICATOR_LED,0);
  }
  else{
    digitalWrite(INDICATOR_LED,1);
  }

  //while (Genotronex.available() > 0){
  while(Serial.available() > 0){  

  Alarm.delay(1000);
    int command = Serial.parseInt();

    switch (command){
      case 1:
        setArduinoTime(Serial.parseInt(),Serial.parseInt(),Serial.parseInt(),Serial.parseInt(),Serial.parseInt(),Serial.parseInt());
        buzz();
		Serial.println("Time Set to:");
        digitalClockDisplay();
        break;
      case 2:
        digitalClockDisplay();
        break;
      case 3:
       //   alarmHour = Serial.parseInt();
       // alarmMinute = Serial.parseInt();
       // alarmSecond = Serial.parseInt();
       // setArduinoAlarm(alarmHour, alarmMinute, alarmSecond);
	    setArduinoAlarm(Serial.parseInt(), Serial.parseInt(), Serial.parseInt());
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










