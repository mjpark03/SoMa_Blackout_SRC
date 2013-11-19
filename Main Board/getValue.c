#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void getFileData();
void token(char *str,int length);
int main(void){

    while(1)
    {
    	system("./html > html.txt");		//소켓통신으로 htmle 얻기

    	system("grep '<td align=\"r' html.txt > html2.txt");	//현재부하랑, 예비전력량, 예비율 찾기

    	getFileData();
        
        sleep(300);
    }
	return 0;
}



void getFileData(){

	char PowerLoadAmount[100]; 	//현재부하량
	char ReservedAmount[100]; 	//전력 예비량
	char ReservationRate[100];	//예비율
	FILE *fp;
	char seps[] = "<td align=\"right\"></td>"; //자를>    것      
	char *token;
	int ReserveValue;
    int LoadValue;
	float Rate;
	char temp;
	int count=0;
    int count2=0;
    int i;
	char buf[100];
    char buf2[100];

	fp = fopen("html2.txt", "r");
	fgets(PowerLoadAmount, 100, fp);
	fgets(ReservedAmount, 100, fp);
	fgets(ReservationRate, 100, fp);
	fclose(fp);

	fp = fopen("value.txt", "w");
	token = strtok(ReservationRate, seps);
	while(token != NULL){
		fprintf(fp, "%s", token);
		token = strtok(NULL, seps);
	}
	fclose(fp);

    fp = fopen("value2.txt", "w");
    token = strtok(PowerLoadAmount, seps);
    while(token != NULL){
        fprintf(fp, "%s", token);
        token = strtok(NULL, seps);
    }
    fclose(fp);


//숫자만 저장


    
	fp = fopen("value.txt", "r");
	while(!feof(fp)){
		temp = fgetc(fp);
		if(temp > 47 && temp < 58)
        {
			buf[count++] = temp;
        }
	}

    fgets(buf, 100, fp);
    ReserveValue = atoi(buf);

//    fclose(fp);

    count=0;
    fp = fopen("value2.txt", "r");
    while(!feof(fp)){
        temp = fgetc(fp);
        if(temp > 47 && temp < 58)
        {
           buf[count++] = temp;
        }
    }
    
	fgets(buf, 100, fp);
	LoadValue = atoi(buf);

 //   fclose(fp);

    Rate=(float)ReserveValue/(float)LoadValue * 100;

   fp = fopen("value.txt", "w");
    fprintf(fp, "%d\n\r", LoadValue);
    fprintf(fp, "%d\n\r", ReserveValue);
    fprintf(fp, "%f\n\r", Rate);
//    fclose(fp);
	printf("%d\n\r",LoadValue);
    printf("%d\n",ReserveValue);
    printf("%f\n",Rate);
    //system("> value3.txt");
    unlink("html.txt");
    unlink("html2.txt");
    unlink("value2.txt");

	fclose(fp);
}
