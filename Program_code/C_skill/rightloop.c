#include "lib_file.h"

#define TRUE 1
#define FALSE 0

int loopstr(char *t1, int n, char *t2)
{
	char b[256] = "";
	int m;
	int i = 0;
	for(i = 0;i<9;i++)
	{
		m = strlen(t1) - i;
		strcpy(b,t1+i);
		strcpy(b + m, t1);
		printf("b + m = %s\n",b+m);
		*(b + strlen(t1)) = '\0';
		strcpy(t1,b);
		printf("t1 = %s\n",t1);
		if(!strcmp(t1,t2))
			return TRUE;
		b[0] = '\0';
	}
	return FALSE;
}

int rightloop(char *t1, char *t2, int n)
{
	int i = 0,j = 0,k = 0;

	int m; 
	char b[100];
	i = strlen(t1);

	while(t1[n] != '\0')
	{
		printf("t1 n = %c\n",t1[n]);
		b[j] = t1[n];
		j++;
		n++;
	}
	printf("j = %d\n",j);

	while(t1[k] != '\0')
	{
		b[j] = t1[k];
		j++;
		k++;
	}

	b[i] = '\0';

	strcpy(t1,b);

	printf("t1 = %s\n",t1);
}

int main()
{
	char *t1 = "abkdk";
	char *t2 = "kdkab";
	char tt[strlen(t1)];
	strcpy(tt,t1);
	rightloop(tt,t2,2);
	return 0;
}
