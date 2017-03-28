#include "lib_file.h"

#define HEIGHT 12

int combi(int r,int n)
{
	int p = 1;
	int i;
	for(i = 1; i<=n;i++)
	{
		p = p * (r - i + 1) / i;
	}
	return p;
}


int main()
{
	int r;
	for(r = 0;r < HEIGHT; r++)
	{
		char format[5];
		sprintf(format, "%%%ds", (HEIGHT - r) * 3);
		printf(format, "");
		int n;
		for(n = 0;n <= r; n++)
		{
			printf("%6d",combi(r,n));
		}
		printf("\n");
	}
	return 0;
}
