
/*
IRreceive

<IR receiver : LTOP-ML38ATH>
1 : Vout (pin2)
2 : GND
3 : Vcc

<IR sender : EL-7L>
long : Vcc (pin3)
short : GND
*/

volatile unsigned long time,new_time,old_time;  
volatile unsigned char ir_check=1, IR_start=0; 
volatile unsigned long mask=0x00000001, IR_rx_data=0x00000000, IR_code; 
 
void setup(){
  Serial.begin(9600);
  attachInterrupt(0, IR_remote, FALLING);
}

void loop(){
  while(ir_check);
  ir_check=1;
}

void IR_remote()  
{
  new_time = micros();          // 현재시간 저장
  time=new_time - old_time;     // 엣지 사이의 시간 계산
  old_time=new_time;            // 이전시간을 현재시간으로 갱신
  
  if(time > 13000 && time <14000) IR_start=1;   // lead code  13.5ms의 리드코드를 확인
  else if(time > 9000 && time <12000)           // repeat code 일때는 무시 입력은 1번만
  {
    Serial.println("R");      // repeat code 가 들어오면 LCD에 'R' 을 출력
    IR_start=0;                // 변수 초기화
    mask=0x00000001;
    IR_rx_data=0x00000000;
  } 
  else if(IR_start==1)         // 리드코드 
  {
    if(time > 1025 && time < 1225)          // 1.125ms = 0
    {
      IR_rx_data &= ~mask;
      mask=mask<<1;       // mask를 1비트씩 Shift
    }
    else if(time > 2125 && time < 2325)     // 2.25ms = 1
    {     
      IR_rx_data |= mask;
      mask=mask<<1;       // mask를 1비트씩 Shift
    }    
                
    if(mask==0)        // mask가 모두 시프트되어 0이되면(32bit)
    {       
      IR_code=IR_rx_data;
      Serial.print(IR_code, HEX);  // IR code 값 시리얼로 전송
      Serial.print(" : ");
      Serial.print(IR_code, BIN);
      Serial.println(" ");
      
      // 변수 초기화                                                                        
      IR_start=0; 
      mask=0x00000001;
      IR_rx_data=0x00000000;     
      
      ir_check=0;
    }  
  }
  
}
