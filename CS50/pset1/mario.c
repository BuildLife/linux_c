#include <stdio.h>

int main()
{
	int high = 0;
	int i = 0, j = 0, x = 0;

	do{
		printf("Enter a number:\n");
		scanf("%d",&high);
	}while(!high);

	for(i = 0; i < high; i++)
	{
		for(j = 0; j < (high - i); j++)
		{
			printf(" ");
		}
		
		for(x = 0; x <= i; x++)
		{
			printf("*");
		}
		
		printf(" ");
		
		for(x = 0; x <= i; x++)
		{
			printf("*");
		}

		printf("\n");
	}
	return 0;
}
