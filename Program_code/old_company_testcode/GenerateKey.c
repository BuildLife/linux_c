#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int CompareAllNumber(char*);


int main()
{
	char test[10],*key;
	int flag = 0;
	key = malloc(sizeof(test));
	do{
		scanf("%s",key);
		flag = CompareAllNumbers(key);

		if( strlen(key) != 10 || flag == 1 )
		{
			printf("it's not the cmc sn format, please enter it again.\n");
		}
		else
			printf("Your CMC SN is : %s\n",key);
		
	}while( strlen(key) != 10 || flag == 1 );


	return 0;
}

int CompareAllNumbers(char *SN)
{
	int i = 0;
	int num = 0, check = 0;
	for(; i<strlen(SN);i++)
	{
		num = *(SN + i) & 0xff;
		if( num < 48 || num > 57 )
		{
			return 1;
		}
	}
	return 0;
}
