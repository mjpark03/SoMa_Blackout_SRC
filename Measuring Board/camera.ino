#include <SD.h>
#include <SoftwareSerial.h>
#define filename "jg.jpg"
#define MAX_RECEIVE_NUM 20
#define DATA_COUNT 16
#define DLAY_TIME 100
#define rxPin 2
#define txPin 3
//const int chipSelect = 4;

///////////Send File////////////////
void transferPacket(byte data[]);
void transferPhoto();
void transferEND();
boolean checkTransfer();
boolean checkReady();


///////////Take Picture/////////////
void takePicture();
void SendResetCmd();
void SendTakePhotoCmd();
void SendReadDataCmd();
void StopTakePhotoCmd();


void fileRemove();
void sendPicture();

///////////Send File////////////////
File myFile;
byte Tpacket[50];
byte Rpacket[30];
byte count=0;
byte ReceivedCount=-1;


///////////Take Picture/////////////
byte incomingHEX;
SoftwareSerial mySerial(rxPin,txPin);                     //Configure pin 4 and 5 as soft serial port
int a=0x0000,j=0,k=0,picture_count=0;                    //Read Starting address       
uint8_t MH,ML;
boolean EndFlag=0;
boolean FinishFlag=0;

int index=30;

void setup()
{ 
  ///////////Send File////////////////
  Serial.begin(9600);
  
  initSD();

  ///////////Take Picture/////////////
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  pinMode(SS,OUTPUT);
  mySerial.begin(38400);

}



void loop() 
{
  delay(100);
  fileRemove(); //이미존재하는 파일이 있으면 삭제
  takePicture(); // 카메라로 사진 찍기
  sendPicture(); //사진 파일 보내기
}

void sendPicture()
{
  if(myFile.available() > 0)
    transferPhoto();
  else{
    //transferPacket(0xff, 0xd9); // transfer end
    myFile.close();
  }

}

void fileRemove()
{
  if(SD.exists(filename))
    SD.remove(filename);
}

void initSD()
{
  if(SD.begin(4))
      Serial.println("SD begin success");
      
   if(!SD.exists(filename)) {
      myFile = SD.open(filename,FILE_READ);
      myFile.close();
  }
  
  myFile = SD.open(filename,FILE_READ);
  if(myFile==false) {
      Serial.println("my file open fail"); 
  }
}

void transferPacket(byte data[])
{
  byte checksum;
  Tpacket[0] = 0xff;
  Tpacket[1] = 0xff;
  Tpacket[2] = 2;
  for(int i=0; i<DATA_COUNT; i++)
    Tpacket[i+3] = data[i];
  Tpacket[3+DATA_COUNT] = count;

  checksum=0;
  for(int i=0; i<2+DATA_COUNT; i++)
    checksum += Tpacket[2 + i];

  Tpacket[4+DATA_COUNT]=~checksum;

  Serial.flush();
  for(int i=0; i<5+DATA_COUNT; i++)
  {
    delay(50);
    Serial.write(Tpacket[i]);
  }

  delay(DLAY_TIME);
}

boolean checkTransfer()
{
  byte checksum;

  index=MAX_RECEIVE_NUM;
  
  delay(DLAY_TIME);
  for(int i=0; i<MAX_RECEIVE_NUM; i++)
    Rpacket[i] = Serial.read();

 
  for(int i=0; i<MAX_RECEIVE_NUM-6; i++)
  {
    if(Rpacket[i] == 0xff && Rpacket[i+1] == 0xff && Rpacket[i+2] == 2)
    {
      index = i;
      break;
    }
  }

  checksum = Rpacket[index + 5] + Rpacket[index + 4] + Rpacket[index + 3] + Rpacket[index + 2];
  checksum = (~checksum)&0xff;

  if(index > MAX_RECEIVE_NUM-7) 
    return false;

  if(Rpacket[index + 6] != checksum)
    return false;
 
  
  if(Rpacket[index + 3] == 0x6e && Rpacket[index + 4] == 0x6f)
    return false;
  else if(Rpacket[index + 3] == 0x6f && Rpacket[index + 4] == 0x6b)
  {
    
    ReceivedCount=Rpacket[index + 5];
     if((count == ReceivedCount-1) || (count == 9 && ReceivedCount == 0))
    {    
      count = (++count)%10;
    
      return true;
    }
  }

  return false;
}


void transferPhoto()
{
  byte data[DATA_COUNT];

  for(int i=0; i<DATA_COUNT; i++)
    data[i]=myFile.read();

  transferPacket(data);

  while(1)
  {
    if(Serial.available()>0)
    {
      if(checkTransfer()) break;
    }
    transferPacket(data);
  }
  
}


void takePicture()
{


  Serial.println("Picture Ready");

  SendResetCmd();
  delay(4000);                               //After reset, wait 2-3 second to send take picture command

  SendTakePhotoCmd();
  
  // add lkwook
  //SD.begin(chipSelect); //or SD.begin(cspin number ex. 10)
  if(SD.begin(4))
      Serial.println("SD begin success");
  
  Serial.println(SS);
  File myFile;
  if(!SD.exists(filename)) {
      myFile = SD.open(filename,FILE_WRITE);
      myFile.close();
  }
  
  delay(3000);
  
  myFile = SD.open(filename,FILE_WRITE);
  if(myFile==false) {
      Serial.println("my file open fail"); 
  }
  //if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
/*
  // open the file for write at end like the Native SD library
  if (!myFile.open("camera.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
  */
  Serial.print("Start");
  
  while(mySerial.available()>0)
  {
    incomingHEX=mySerial.read();
    //Serial.print("count1 : ");
   // Serial.println(count);
    picture_count++;
  } 

  byte a[32];
  //Serial.println("test2");
  while(!EndFlag)
  {  
    j=0;
    k=0;
    picture_count=0;
    SendReadDataCmd();
    //Serial.print(mySerial.read());
    delay(25);
   // Serial.println("test3");
    while(mySerial.available()>0)
    {
      
      incomingHEX=mySerial.read();
      k++;
      if((k>5)&&(j<32)&&(!EndFlag))
      {
        a[j]=incomingHEX;
        if((a[j-1]==0xFF)&&(a[j]==0xD9))      //Check if the picture is over
          EndFlag=1;
        myFile.write(a[j]);         
        j++;
        picture_count++;
      }
    }
      //Send jpeg picture over the serial port

  }      
  //while(1);
  myFile.close();
  Serial.print("Done");
}


//Send Reset command
void SendResetCmd()
{
  byte temp1 = 0x56;
  mySerial.write(temp1);
  temp1 = 0x00;
  mySerial.write(temp1);
  temp1 = 0x26;
  mySerial.write(temp1);
  temp1 = 0x00;
  mySerial.write(temp1);
}

//Send take picture command
void SendTakePhotoCmd()
{
  byte temp2 = 0x56;
  mySerial.write(temp2);
  temp2 = 0x00;
  mySerial.write(temp2);
  temp2 = 0x36;
  mySerial.write(temp2);
  temp2 = 0x01;
  mySerial.write(temp2);
  temp2 = 0x00;
  mySerial.write(temp2);
}

//Read data
void SendReadDataCmd()
{
  MH=a/0x100;
  ML=a%0x100;
  
  byte temp3 = 0x56;
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = 0x32;
  mySerial.write(temp3);
  temp3 = 0x0c;
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = 0x0a; 
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = MH;
  mySerial.write(temp3);
  temp3 = ML;
  mySerial.write(temp3);
  temp3 = 0x00;  
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = 0x20;
  mySerial.write(temp3);
  temp3 = 0x00;
  mySerial.write(temp3);
  temp3 = 0x0a;  
  mySerial.write(temp3);
  a+=0x20;                            //address increases 32，set according to buffer size
}

void StopTakePhotoCmd()
{
  byte temp4 = 0x56;
  mySerial.write(temp4);
  temp4 = 0x00;
  mySerial.write(temp4);
  temp4 = 0x36;
  mySerial.write(temp4);
  temp4 = 0x01;
  mySerial.write(temp4);
  temp4 = 0x03;
  mySerial.write(temp4);        
}

