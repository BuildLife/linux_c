#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
	char *Tchar = malloc(sizeof(char) * 128);
	char *Fchar = malloc(sizeof(char) * 128);
	int Cpare = 0;
	
	printf("Select the function of option : \n");
	printf("The First char: \n");
	scanf("%s",Tchar);
	printf("The Second char: \n");
	scanf("%s",Fchar);
	
	printf("Input the compare char word number : \n");
	scanf("%d",&Cpare);


	if(!(strncmp(Tchar,Fchar,Cpare)))
	{
		printf("if!(strncmp(%s,%s,compare the same how many char word : ex 6 : compare the 6 char word ))\n",Tchar,Fchar);
		printf("!strncmp return 1\n");
	}
	else
		printf("strncmp return 0\n");
	

	return 0;
}
