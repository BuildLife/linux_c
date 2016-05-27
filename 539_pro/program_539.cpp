#include<stdio.h>
#include<stdlib.h>
#include<time.h>


#define Max_Number 5
#define NELEMS(x)  (sizeof(x)/(sizeof(x)[0]))

void Include_All(int *, int, int*); //collect all array 
void Change_List(int *); //List from small to large

<<<<<<< HEAD
//1234566
=======
//TEST
>>>>>>> 7ea42c781fa40f8a9189f71cfcb5fef8de872814
int main()
{
	int NumberOne,NumberTwo,NumberThree,NumberFour,NumberFive;
	int x = 0;
	int y = Max_Number;
	int k;

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

	Change_List(Complete);
	for(k=0;k<Max_Number;k++)
	{
		printf("%d,",Complete[k]);
	}
	return 0;
}

/*collect all array*/
void Include_All(int *Com,int rNum,int *s)
{
	*(Com + *s) = rNum;
	*s+=1;
}
void Change_List(int *List)
{
	int j=0;
	int i=0;
	int x = 0;
	int temp;
	for(i=0;i<Max_Number-1;i++)
	{
		for(j=i+1;j<Max_Number;j++)
		{
			if( *(List+j) < *(List+i) )
			{
				temp = *(List+j);
				*(List+j) = *(List+i);
				*(List+i) = temp;
			}
		}	
	}
}
