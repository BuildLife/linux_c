#include<stdlib.h>
#include<stdio.h>
#include "lib_file.h"
#define JAN_1970 2208988800
int main()
{
	FILE *fp = NULL;
	unsigned char ntpstr[6][100] = {0};
	int l = 0;
	int x = 0;
	unsigned char t_day[100] = {0};
	unsigned char t_sec[100] = {0};
	unsigned int ti_day = 0;
	unsigned int td_sec = 0;
	fp = fopen("/home/ericchen/timetxt","r");

	memset(ntpstr, 0x00, sizeof(ntpstr));
	for(l = 0;l<3;l++)
	{
		fgets((char*)ntpstr[l],sizeof(ntpstr),fp);
	}
	fclose(fp);
	int t = 0,d = 0;
	int flag = 0;
	for(x = 0;x < 18;x++)
	{
		if(isspace(ntpstr[2][x]))
			flag = 1;
		else if(flag == 0)	
			t_day[t++] = ntpstr[2][x];
		else if(flag == 1)
			t_sec[d++] = ntpstr[2][x];
	}
	printf("%s\n",t_day);
	printf("%s\n",t_sec);
	
	ti_day = atoi(t_day);
	td_sec = atoi(t_sec);
	time_t timep,cutime;
	time(&timep);
	struct tm *tmp;
	struct tm *tt;
	tmp = localtime(&timep);
	printf("%d\n",tmp->tm_year);
	printf("%d\n",tmp->tm_mon + 1);
	printf("%d\n",tmp->tm_mday);
	printf("%d\n",tmp->tm_hour);
	printf("%d\n",tmp->tm_min);
	printf("%d\n",tmp->tm_sec);
	int year = tmp->tm_year + 1900;

	//cutime = (time_t)((ti_day * 86400) + td_sec - JAN_1970);
	cutime = (time_t)((ti_day * 86400) + td_sec - JAN_1970);
	//cutime = strtoul((char*)ti_day,NULL,0);
	//cutime = strtoul("1590147900",NULL,0);
	tt = localtime(&cutime);
	printf("%d\n",tt->tm_year + 1900);
	printf("%d\n",tt->tm_mon + 1);
	printf("%d\n",tt->tm_mday);
	printf("%d\n",tt->tm_hour);
	printf("%d\n",tt->tm_min);
	printf("%d\n",tt->tm_sec);

	printf("tmp %d\n",tmp->tm_year);
	printf("tt %d\n",tt->tm_year);
	int tyear = tt->tm_year + 1900;
	
	if(year != tyear)
	{
		printf("year was different\n");
	}
	else
		


	return 0;
}
