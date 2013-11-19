#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

//#include <linux/delay.h>

#define PORT1	"/dev/ttyUSB0"

#define BAUDRATE    9600	


void transferPacketR(unsigned char, unsigned char);



//---------------------------------------------------------

int OpenSerial(char *device_name);
int CloseSerial(int fd);

int FdPort1;
unsigned char Tpacket[10];



int main( void)
{        
   int    fd;
   int    ndx;
   int    cnt;
   char   buf2[1024]="bb";
   char   buf[1024];

    FILE *fp;
    char buffer[10];
char temp;
    int i=0;
    int stateFlag=1; //1==on, 0==off
    int stateFlag2=1; //1==up, 0==down


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


  

   while ( 1)
   {
	 fp = fopen("sete.txt", "r");

        while(!feof(fp))
        {
            temp = fgetc(fp);
            if(temp>='a' && temp<='z')
                buffer[i++] = temp;
        }
        
    
        if(buffer[0]=='o'&&buffer[1]=='f'&&buffer[2]=='f')
        {
            if(stateFlag == 1)
            {
                transferPacketR('o','f');
                stateFlag=0;
                printf("a");
                fflush(stdout);
            }
        }
        else if(buffer[2]=='o'&&buffer[3]=='n')
        {
            if(stateFlag2 == 1)
            {
                transferPacketR('d','w');
                stateFlag2=0;
                printf("b");
                fflush(stdout);
            }
        }
        i=0;
    fclose(fp);
    usleep(1000);
//	fclose(fp);


   }

   close(FdPort1);
   return 0;
}



int CloseSerial(int fd)
{
		close(fd);
}

void transferPacketR(unsigned char data1, unsigned char data2)
{
	 unsigned char checksum;
        int i;
     
        Tpacket[0] = 0xdd;
        Tpacket[1] = 0xdd;
        Tpacket[2] = data1;
        Tpacket[3] = data2;
        checksum = Tpacket[2] + Tpacket[3];
        Tpacket[4] = ~checksum;
        
        for(i=0; i<5; i++)
             write(FdPort1, Tpacket+i, 1);
        
}

