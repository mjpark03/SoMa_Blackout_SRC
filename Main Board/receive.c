#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

//#include <linux/delay.h>

#define PORT1	"/dev/ttyUSB0"
#define PORT2	"/dev/ttyS0"
#define BAUDRATE    9600	

//---------------------------------------------------------
#define filename "jg.jpg"
#define MAX_RECEIVE_NUM 50
#define DATA_COUNT 16
#define DELAY_TIME 30

#define bool int
#define true 1
#define false 0


void transferPacket(unsigned char, unsigned char);
void transferPhoto();

//void checkSTART(unsigned char data1, unsigned char data2);
//void checkEND(unsigned char, unsigned char);
bool receiveData();

void saveData();
//---------------------------------------------------------

int OpenSerial(char *device_name);
int CloseSerial(int fd);
void *AnotherPortReceive(void *fd);

void initFILE();
void initValue();

int ReceiveCount=-1;
int FdPort1;
int FdPort2;
FILE *fJPEG;
unsigned char count=0;
unsigned char ReceivedCount=1;
unsigned char end1, end2;
unsigned char Tpacket[10];
unsigned char Rpacket[50];

int StartIndex;

bool StartFlag, StartFlag_Flag, EndFlag;
bool ReceiveFlag=1;


int main( void)
{        
   int    fd;
   int    ndx;
   int    cnt;
   char   buf2[1024]="bb";
   char   buf[1024];
   struct termios    newtio;
   //struct pollfd     poll_events;      // 체크할 event 정보를 갖는 struct
   int    poll_state;

   // 시리얼 포트를 open

   FdPort1 = open( "/dev/ttyUSB0", O_RDWR | O_NOCTTY );// | O_NONBLOCK );        // 디바이스를 open 한다.

   if ( 0 > FdPort1)
   {        
      printf("open error\n");
      return -1;
   }

   // 시리얼 포트 통신 환경 설정

   memset( &newtio, 0, sizeof(newtio) );
   newtio.c_cflag       = B9600 | CS8 | CLOCAL | CREAD;
   newtio.c_oflag       = 0;
   newtio.c_lflag       = 0;
   newtio.c_iflag       = IGNPAR;// | ICRNL;
   newtio.c_cc[VTIME]   = 1;
   newtio.c_cc[VMIN]    = 1;
   
   tcflush  (FdPort1, TCIFLUSH );
   tcsetattr(FdPort1, TCSANOW, &newtio );



   // 자료 수신
   initFILE();
   initValue();
  // unlink(filename);

   while ( 1)
   {

   	if(ReceiveFlag==1)
	{
		transferPacket('o', 'k');

		if(receiveData())
		{
			saveData();
			transferPacket('o', 'k');
		}
	}
	else
	{
		system("./gocr jg.jpg > watt.txt");
		unlink(filename);
//		ReceiveFlag=1;
		initValue();
		initFILE();
	}

   }

   close(FdPort1);
   return 0;
}



int OpenSerial(char *device_name)
{
	int fd;
	struct termios newtio;

	fd = open(device_name, O_RDWR | O_NOCTTY);

	if(fd < 0)
	{
			printf("Serial Port Open Fail.\n");
			return -1;
	}

	memset(&newtio, 0, sizeof(newtio));
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_cflag = CS8|CLOCAL|CREAD;

	switch(BAUDRATE)
	{
			case 115200 : newtio.c_cflag |= B115200;
						 break;
			case 57600 : newtio.c_cflag |= B57600;
						 break;
			case 38400 : newtio.c_cflag |= B38400;
						 break;
			case 19200 : newtio.c_cflag |= B19200;
						 break;
			case 9600 : newtio.c_cflag |= B9600;
						 break;
			default : newtio.c_cflag |= B57600;
						 break;
	}

	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 20;
	newtio.c_cc[VMIN] = 50;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	return fd;


}

void initValue()
{
	StartFlag_Flag=true;
	StartFlag=false;
	EndFlag=false;
	count=0;
	ReceiveFlag=1;
}

int CloseSerial(int fd)
{
		close(fd);
}

void *AnotherPortReceive(void *fd)
{
		int FileDescriptor;
		int ReadMsgSize = 0;
		unsigned char DataBuf[50];

		memset(DataBuf, 0, 50*sizeof(unsigned char));
		FileDescriptor = (int)fd;

		while(1)
		{
			ReadMsgSize = read(FileDescriptor, DataBuf, 1);
			if(ReadMsgSize > 0)
					printf("Zigbee Module : %c |%d| Byte \n", DataBuf[0], ReadMsgSize);
		}
}

void saveData()
{
	int i;

	int index = 30;

	if(count == ReceiveCount)
	{
        printf("save2");
        fflush(stdout);
		count = (++count)%10;

		for(i=0; i<DATA_COUNT; i++)
				fputc(Rpacket[StartIndex + 3 + i], fJPEG);
	}
}

void transferPacket(unsigned char data1, unsigned char data2)
{
	unsigned char checksum;
	int i;

	Tpacket[0] = 0xff;
	Tpacket[1] = 0xff;
	Tpacket[2] = 0x02;
	Tpacket[3] = data1;
	Tpacket[4] = data2;
	Tpacket[5] = count;

	checksum = Tpacket[2] + Tpacket[3] + Tpacket[4] + Tpacket[5];

	Tpacket[6] = ~checksum;
	printf("ok");
	fflush(stdout);
	//fflush(stdin);
	for(i=0; i<7; i++)
	    write(FdPort1, Tpacket+i, 1);

	usleep(1000);
	//delay(25);
}

void checkSTART(unsigned char data1, unsigned data2)
{
     char p[20] = "check start\n\r";
     if(StartFlag_Flag)
     {
         write(FdPort1, p, sizeof(p));
 
         if(data1 == 255 && data2 == 216)
         {
             StartFlag_Flag = false;
             StartFlag = true;
         }
         else
             StartFlag = false;
     }
}
 
void checkEND(unsigned char data1, unsigned data2)
{
     char p[10] = "end\n\r";
     if(data1 == 255 && data2 == 217)
     {
 
         write(FdPort1, p, sizeof(p));
         fclose(fJPEG);
		 ReceiveFlag=0;
		 printf("end");
		 fflush(stdout);
     }
}


bool receiveData()
{
	int i;
	//unsigned char checksum;
	int index = 50;
	unsigned char checksum;

    int cnt=0;

	//printf("debug");
	//fflush(stdout);
	//fflush(stdin);
	//mdelay(DELAY_TIME);
	
	usleep(500);
	//for(i=0; i<50; i++)
    memset(Rpacket, 0, 50);
	for(i=0; i<50; i++)
    {
        read(FdPort1, Rpacket+i, 1);
        cnt++;
    }

    //read(FdPort1, Rpacket, 50);
    /*for(i=0; i<50; i++)
    {
        if(Rpacket[i] == 128)
            Rpacket[i] = 0;
    }*/

	for(i=0; i<MAX_RECEIVE_NUM; i++)
	{
		if(Rpacket[i] == 255 && Rpacket[i+1] == 255 && Rpacket[i+2] == 2)
		{
				index =i;
				StartIndex=i;
				break;
		}
	}
	
   // Rpacket[49] = 255;
    printf("\n\rindex : %d ", index); 
	for(i=0; i<50; i++)
        printf("%d ", Rpacket[i]);
	fflush(stdout);

	checksum=0;

	for(i=0; i<2+DATA_COUNT; i++)
			checksum += Rpacket[index + i + 2];

    printf("checksum: %d", checksum);
    fflush(stdout);
	checksum = (~checksum)&0xff;

	if(index > MAX_RECEIVE_NUM) return false;

    printf("a%d",checksum);
    fflush(stdout);
	if(Rpacket[index + 4 + DATA_COUNT] == checksum)
	{
		ReceiveCount = Rpacket[index + (5+14)];
        printf("Rcnt : %d", ReceiveCount);

		for(i=0; i<DATA_COUNT-1; i++)
		{
			checkSTART(Rpacket[index + 3 + i], Rpacket[index + 4 + i]);
			checkEND(Rpacket[index + 3 + i], Rpacket[index + 4 + i]);
		}

        printf("b");
        fflush(stdout);
		return true;
	}
	else
		return false;
}


void initFILE()
{
	fJPEG = fopen(filename, "wr");
}
