#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define Max_Number 5
#define NELEMS(x)  (sizeof(x)/(sizeof(x)[0]))

void Include_All(int *, int, int*);

int main()
{
	int NumberOne,NumberTwo,NumberThree,NumberFour,NumberFive;
	int x = 0;
	int k;
	//int *i = &x;
	int ItemOne[] = {7,9,21,22,28,33,34,38,1,5};
	int ItemTwo[] = {8,13,27,10,17,19,20,23,29,36};
	int ItemThree[] = {2,3,4,11,14,16,24,25,26,35};
	int ItemFour[] = {6,15,18,30,31,37,39,12,32};

	int Complete[Max_Number];
	
	srand(time(NULL));

	NumberOne = (rand()%NELEMS(ItemOne));	
		Include_All(Complete,ItemOne[NumberOne],&x);
		
	NumberTwo = (rand()%NELEMS(ItemTwo));
		Include_All(Complete,ItemTwo[NumberTwo],&x);

	NumberThree = (rand()%NELEMS(ItemThree));
		Include_All(Complete,ItemThree[NumberThree],&x);
	
	NumberFour = (rand()%NELEMS(ItemFour));
		Include_All(Complete,ItemFour[NumberFour],&x);

	NumberFive = (rand()%39)+1;
		Include_All(Complete,NumberFive,&x);

	
	for(k=0;k<Max_Number;k++)
	{
		printf("%d,",Complete[k]);
	}
	return 0;
}


void Include_All(int *Com,int rNum,int *s)
{
	*(Com + *s) = rNum;
	*s+=1;
}
