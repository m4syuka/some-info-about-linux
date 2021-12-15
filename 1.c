#include <stdio.h>
#include <sys/unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <sys/utsname.h>
#include <dirent.h>

int main()
{
	const double Gb = 1024 * 1024 * 1024;
	
	//Имя хоста и дистрибутив
	struct utsname distrName;
	uname(&distrName);
	printf("servername: %s\nrelease: %s %s\n",distrName.nodename,distrName.sysname,distrName.release);
	
	//ЦП
	FILE *cpuinfo = fopen("/proc/cpuinfo","rb");
	char buffCPU [1024] = {0};
	do
	{
		fgets(buffCPU,1024,cpuinfo);
	}while(strstr(buffCPU,"model name") == 0);
	printf("cpu %s",buffCPU);
	
	//Оперативная память
	struct sysinfo info;
	sysinfo(&info);
	printf("memory	: %4.1fGb / %4.1fGb\n",(info.totalram / Gb) - (info.freeram / Gb),info.totalram / Gb);	

    //Сеть
    const char dirNet [] = "/sys/class/net"; 
    DIR *dirSysClassNet = opendir(dirNet);
    struct dirent *de;    
    while(dirSysClassNet)
    {
        de = readdir(dirSysClassNet);
        if(!de)
            break;
        //Игнорируем . .. lo        
        if(de->d_type != 4 && (strstr(de->d_name,"lo") == 0))
        {
			char fileNetOne [256] = {0}, fileNetTwo [256] = {0};
            //Выводим имя интерфейс
			printf("%s -  ",de->d_name);
            //прокладываем путь до папки ../statistics
			strcat(fileNetOne,dirNet);            
            strcat(fileNetOne,"/");
            strcat(fileNetOne,de->d_name);
            strcat(fileNetOne,"/statistics/");
            strcpy(fileNetTwo,fileNetOne);  
			//открываем папку
            DIR *dirStatistics = opendir(fileNetOne);         
            if((de = readdir(dirStatistics)) != 0)
            {
                strcat(fileNetOne,"rx_bytes");//in
                strcat(fileNetTwo,"tx_bytes");//out             
                FILE *fileSpeed = fopen(fileNetOne,"rb");
                long int one = 0,two = 0;
                //читаем файлик первый рах
				fscanf(fileSpeed,"%ld",&one);
                sleep(1);
                fclose(fileSpeed);
                fileSpeed = fopen(fileNetOne,"rb");
                //читаем второй раз
				fscanf(fileSpeed,"%ld",&two);
                printf("network in:%.2f Mbit/s ",((two - one) * 8)/(1024.* 1024.));
                fclose(fileSpeed);                
                
				//аналогичная штука
                one = 0;
                two = 0;                
                fileSpeed = fopen(fileNetTwo,"rb");
                fscanf(fileSpeed,"%ld",&one);
                sleep(1);
                fclose(fileSpeed);
                fileSpeed = fopen(fileNetTwo,"rb");
                fscanf(fileSpeed,"%ld",&two);
                printf("out:%.2f Kbit/s\n",((two - one)*8)/1024.);

            }
        }
    }

    //Материнская плата
    FILE * mBoard = fopen("/sys/devices/virtual/dmi/id/board_vendor","rb");
    char buffmBoard [1024] = {0};
    fgets(buffmBoard,1024,mBoard);
    buffmBoard[strlen(buffmBoard) - 1] = ' ';
    printf("motherboard: %s",buffmBoard);

    mBoard = fopen("/sys/devices/virtual/dmi/id/board_name","rb");
    fgets(buffmBoard,1024,mBoard);
    printf("%s",buffmBoard);	
	
	
	return 0;
}
