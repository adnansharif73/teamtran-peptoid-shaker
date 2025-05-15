#include <SimpleTimer.h> 
SimpleTimer timer;
bool flag = 0;
int speedPin=5;
int dir1=4;
int dir2=3;
//Anything between 0-255 will ditact the speed of the motor
int mSpeedON=255;
int mSpeedOFF=0;


void setup() {
Serial.begin(96000);
pinMode(speedPin,OUTPUT);
pinMode(dir1,OUTPUT);
pinMode(dir2,OUTPUT);
timer.setInterval(900000, arrosage);

}

void loop() {
  // put your main code here, to run repeatedly:
timer.run(); 
}

void arrosage() {
   if (flag == 0 ){

digitalWrite(dir1,HIGH);
digitalWrite(dir2,LOW);
analogWrite(speedPin,mSpeedON);
flag = 1; 
   } else {
analogWrite(speedPin,mSpeedOFF);
flag = 0;
   }

}
