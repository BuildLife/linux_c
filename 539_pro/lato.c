#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
	int latonum[50] = {1,2,3,4,5,6,7,8,9,10,
					   11,12,13,14,15,16,17,18,19,20,
					   21,22,23,24,25,26,27,28,29,30,
					   31,32,33,34,35,36,37,38,39,40,
					   41,42,43,44,45,46,47,48,49};

void listpair(int *out,int size)
{
	int x = 0,j = 0,tmp = 0;
	for(x = 0;x<size;x++)
	{
		for(j = 0;j<size;j++)
		{
			if(*(out + j) > *(out+x))
			{
				tmp = *(out+j);
				*(out+j) = *(out+x);
				*(out+x) = tmp;
			}
		}
	}
}

void lato_exclude()
{
	int outnum[49];

	int x = 0,i = 0;
	int temp = 0;
	do
	{
		scanf("%d",&temp);
		if(temp < 50 && latonum[temp-1] != 0)
		{
			latonum[temp-1] = latonum[temp-1] - temp;
			outnum[i++] = temp;
		}
	}while(temp < 100);
	
	
	listpair(outnum,i);
	printf("out number:\n");
	int q = 0;
	for(q = 0;q<i;q++)
	{
		printf("%d,",outnum[q]);
	}

	printf("\n");
	printf("It current stays numbers:\n");
	for(x = 0;x<sizeof(latonum)/sizeof(int);x++)
	{
		if(latonum[x] != 0)
		{
			printf("%d ",latonum[x]);
		}
		if(x == 9 || x == 19 || x == 29 || x == 39)
			printf("\n");
	}
	printf("\n");
}


/*********************************************************

					Linear Regression

***********************************************************/

/*double data1[12][2] = {
	//X  Y
	{187.1, 25.4},
	{179.5, 22.8},
	{157.0, 20.6},
	{197.0, 21.8},
	{239.4, 32.4},
	{217.8, 24.4},
	{227.1, 29.3},
	{233.4, 27.9},

};*/

double data1[12][2] = {
	{1,8},
	{3,4},
	{5,6},
	{7,2},
	{9,10},
	{11,12},
	{13,14},
	{15,16},
	{17,18},
	{19,20},
	{21,22},
	{23,24},
};

int LinearRegression(double* data, int rows, double *a, double *b, double *SquarePoor)
{
	int m;
	double *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;

	if(data == 0 || a == 0 || b == 0 || rows < 1)
		return -1;
	
	for(p = data, m = 0; m < rows; m++)
	{
		xa += *p++;
		ya += *p++;
	}

	xa /= rows;
	ya /= rows;

	for(p = data, m = 0; m < rows; m++, p += 2)
	{
		Lxx += ((*p - xa) * (*p - xa));
		Lxy += ((*p - xa) * (*(p + 1) - ya));
	}

	*b = Lxy / Lxx;
	*a = ya - *b * xa;

	if(SquarePoor == 0)
		return 0;

	SquarePoor[0] = SquarePoor[1] = 0.0;

	for(p = data, m = 0; m < rows; m++, p++)
	{
		Lxy = *a + *b * *p++;
		SquarePoor[0] += ((Lxy - ya) * (Lxy - ya));
		SquarePoor[1] += ((*p - Lxy) * (*p - Lxy));
	}

	SquarePoor[2] = SquarePoor[0];
	SquarePoor[3] = SquarePoor[1] / (rows - 2);

	return 0;
}

void display(double *data, double *answer, double *SquarePoor, int rows, int cols)
{
	double v, *p;
	int i,j;
	printf("return program : Y = %.5lf", answer[0]);
	for(i = 0; i < cols; i++)
	{
		printf(" + %.5lf*X%d",answer[i],i);
	}
	printf("\n");
	printf(" ");
	printf("Return checks:\n");
	printf("Return sums : %12.4lf Return sub : %12.4lf\n",SquarePoor[0], SquarePoor[2]);
	printf("Less sums : %12.4lf less sub : %12.4lf\n",SquarePoor[1], SquarePoor[3]);
	printf("dis-sub sums : %12.4lf normal sub : %12.4lf\n",SquarePoor[0] + SquarePoor[1], sqrt(SquarePoor[3]));
	printf("F check sums : %12.4lf with sub : %12.4lf\n",SquarePoor[2] / SquarePoor[3], sqrt(SquarePoor[0] / (SquarePoor[0] + SquarePoor[1])));

	printf("less analysis:\n");
	printf("check value		value	less value		less avaerage\n");
	for(i = 0,p = data; i < rows; i++, p++)
	{
		v = answer[0];
		for(j = 1; j < cols; j++, p++)
			v += *p * answer[j];
		printf("%12.2lf%12.2lf%12.2lf%12.2lf",*p, v, *p-v,(*p - v) * (*p - v));
		printf("\n");
	}
}

int main()
{	
	printf("please enter number\n");
	//lato_exclude();
	double Answer[2], SquarePoor[4];
	if(LinearRegression((double*)data1, 12, &Answer[0], &Answer[1], SquarePoor) == 0)
		display((double*)data1, Answer, SquarePoor, 12, 2);
	return 0;
}
