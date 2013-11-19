
/*

IRsend

<IR receiver : LTOP-ML38ATH>
1 : Vout (pin2)
2 : GND
3 : Vcc

<IR sender : EL-7L>
long : Vcc (pin3)
short : GND
*/

#include <IRremote.h>

#define MAX_RECEIVE_NUM  10

#define DOWN  1
#define OFF 2

#define DLAY_TIME  25

IRsend irsend;
byte Rpacket[MAX_RECEIVE_NUM];
byte irValue;
int index;

void setup()
{
  Serial.begin(9600);
}

void loop() {
  boolean recvData = false;
  
  if (Serial.available()) {
    recvData = receiveData();
    
    if(recvData){
      sendIR();
    }
  }

}

boolean receiveData()
{
  byte checksum;
  index=MAX_RECEIVE_NUM;
  byte rcvResult;
  
  delay(DLAY_TIME);
  
  for(int i=0; i<MAX_RECEIVE_NUM; i++){
    Rpacket[i] = Serial.read();
  }
  
  for(int i=0; i<MAX_RECEIVE_NUM; i++){
    Serial.print(Rpacket[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.flush();
      
  for(int i=0; i<MAX_RECEIVE_NUM-5; i++)
  {
    if(Rpacket[i] == 0xdd && Rpacket[i+1] == 0xdd)
    {
      index = i;
      break;
    }
  }

  checksum=0;
  checksum = Rpacket[index+2] + Rpacket[index+3];
  
  if(checksum == 'd'+'w'){
      irValue = DOWN;
    }else if(checksum == 'o'+'f'){
      irValue = OFF;
    }
    
  checksum = (~checksum)&0xff;

  if(index > MAX_RECEIVE_NUM) return false;

  if(Rpacket[index + 4] == checksum)
  {
    return true;
  }else
    return false;
}

void sendIR(){
      if(irValue == OFF)
        irsend.sendNEC(0x20DF10EF, 34); // input the hex code
      else if(irValue == DOWN)
        irsend.sendNEC(0x20DFC03F, 34); // input the hex code
}
