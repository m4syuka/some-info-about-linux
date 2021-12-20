#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

//для переворота строки
void ReverseStr(char* ch)
{
	int lenStr = strlen(ch)-1;
	for(int i = 0;i<(lenStr/2) + 1;++i)
	{
		char temp = ch[i];
		ch[i] = ch[lenStr - i];
		ch[lenStr - i] = temp;
	}
}
//Функция для обнуления массива
void ZeroArray(char *arr,int len_arr)
{
	for (int i = 0;i<len_arr;++i)
		arr[i] = 0;
}

int main()
{
	const double Gb = 1024 * 1024;
	
	//Имя хоста
	char serverName [256] = {0};
	FILE *serverInfo = fopen("/proc/sys/kernel/hostname","rb");
	fgets(serverName,256,serverInfo);
	printf("servername: %s",serverName);	
	fclose(serverInfo);
	
	//Дистрибутив (Можно, конечно, обратиться к файликам 
	// /proc/sys/kernel/{ostype, osrelease}, но я решил вывести полную инфу
	char serverDistr [1024] = {0};
	serverInfo = fopen("/proc/version","rb");
	fgets (serverDistr,1024,serverInfo);
	printf("release: %s",serverDistr);


	//ЦП
	FILE *cpuinfo = fopen("/proc/cpuinfo","rb");
	char buffCPU [1024] = {0};
	do
	{
		fgets(buffCPU,1024,cpuinfo);
	}while(strstr(buffCPU,"model name") == 0);
	printf("cpu %s",buffCPU);
	
	//Оперативная память
	char memTotalChr [128] = {0},memFreeChr [128] = {0},buff [128] = {0};
	long int memTotalInt = 0, memFreeInt = 0;
	FILE *memFile = fopen("/proc/meminfo","rb");
	//Читаем из файла
	fgets(memTotalChr,128,memFile);
	fgets(memFreeChr,128,memFile);
	//Преобразования, чтобы вытащить кол-во килобайт из памяти
	strcpy(buff,memTotalChr + 10); //удаляем MemTotal
	ReverseStr(buff); //переворачиваем строку, чтобы удалить "\nKb "
	strcpy(memTotalChr,buff + 4);//удаляем их
	char* pos = strstr(memTotalChr, " ");//находим место, где встречается " "
	ZeroArray(buff,128);
	strncpy(buff,memTotalChr,pos - memTotalChr);//копируем цифры в строк
	ReverseStr(buff);//приводим строку к начальному виду
	memTotalInt = strtol(buff,NULL,10);//переводим цифры из строки в int
	//аналогично
	ZeroArray(buff,128);
	strcpy(buff,memFreeChr + 8);
	ReverseStr(buff);
	strcpy(memFreeChr,buff + 4);
	pos = strstr(memFreeChr," ");
	ZeroArray(buff,128);
	strncpy(buff,memFreeChr,pos - memFreeChr);
	ReverseStr(buff);
	memFreeInt = strtol(buff,NULL,10);
	printf("memory (free/total): %4.1fGb/%4.1fGb\n",memFreeInt/Gb,memTotalInt/Gb );

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
