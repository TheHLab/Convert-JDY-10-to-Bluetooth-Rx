/*
 * JDY-10 to Bluetooth Receiver
 * @author TheHLab
 * @created Dec 2, 2020
 * This code for ATTiny85 - 8Mhz internal
 * Tutorial Video: https://youtu.be/QAJTv1-Si7I
 */

#include <NeoSWSerial.h>
#include <SoftwareServo.h> 
NeoSWSerial BTserial(3, 4); // RX | TX

#define TURN_PIN 0
#define GO_PIN 1
#define LIGHT1 2
//#define LIGHT2 4
#define MIN_TURN 0 //700
#define MAX_TURN 179 //2000
#define STOP_SPEED 90 //1500
#define MAX_SPEED_GO 179 //2000
#define MAX_SPEED_BACK 0 //1000
SoftwareServo turn;
SoftwareServo sgo;
char command;
String string;
int svangle = 0;
int slideBarValue = 50;
int index = 0;
String aCmd;

int speeds = STOP_SPEED;
int gear = 0;

void far_light(){
  digitalWrite(LIGHT1, HIGH);
//  digitalWrite(LIGHT2, LOW);
}

void near_light(){
  digitalWrite(LIGHT1, LOW);
//  digitalWrite(LIGHT2, HIGH);
}

void light_off(){
  digitalWrite(LIGHT1, LOW);
//  digitalWrite(LIGHT2, LOW);
}

void setup()
{
  //Serial.begin( 9600 );//115200
  pinMode(1, INPUT);
  //pinMode(TxD, OUTPUT);
  BTserial.begin( 9600 );
  
  turn.attach(TURN_PIN);
  turn.setMinimumPulse(1000);
  turn.setMaximumPulse(2000);
  svangle = map(slideBarValue, 0, 100, MIN_TURN, MAX_TURN);
  turn.write(svangle);
  
  sgo.attach(GO_PIN);
  sgo.setMinimumPulse(1000);
  sgo.setMaximumPulse(2000);
  sgo.write(speeds);
  
  SoftwareServo::refresh();
  
  pinMode(LIGHT1, OUTPUT);
//  pinMode(LIGHT2, OUTPUT);
  //Serial.println("Setup done!");
}

void loop()
{
  //string = "";
  while(BTserial.available() > 0)
  {
    command = ((byte)BTserial.read());
    if(command == ':')
    {
      break;
    }
    else
    {
      string += command;
    }
    //delay(1);
  }
  //if(string != "")  Serial.println(string);
  
  while( string.length() >= 3 ){
      aCmd = string.substring(0, 3);
      //string = string.substring(3);
      //Serial.println(" " + aCmd);

      if( aCmd.lastIndexOf("T") == 0 ){
        // Turn left/right: cmd = "T<value from 0 to 100>"
          slideBarValue = aCmd.substring(index+1).toInt();
          //Serial.println(slideBarValue );
          if( slideBarValue > 0 ){
            svangle = map(slideBarValue, 0, 100, MIN_TURN, MAX_TURN);
            turn.write(svangle);
            SoftwareServo::refresh();
          }
          string = string.substring(3);
      } else if ( aCmd.lastIndexOf("S") == 0 ){
        speeds = aCmd.substring(1).toInt();
        if( speeds > 0 ){
          speeds -= 15;
          if( gear == 3 ){
            sgo.write( map(speeds, 0, 100, STOP_SPEED, MAX_SPEED_GO) );
            SoftwareServo::refresh();
          } else if( gear == 1 ){
            sgo.write( map(speeds, 0, 100, STOP_SPEED, MAX_SPEED_BACK) );
            SoftwareServo::refresh();
          }
          delay(10);
        }
        string = string.substring(3);
      } else if ( aCmd.lastIndexOf("G") == 0 ){
        gear = aCmd.substring(1).toInt();
        string = string.substring(3);
      } else if ( aCmd.lastIndexOf("L") == 0 ){
        if( aCmd == "LFA" ) far_light();
        else if( aCmd == "LNE" ) near_light();
        else light_off();
        string = string.substring(3);
      } else {
        string = string.substring(1);
        //break;
      }
  }
}

