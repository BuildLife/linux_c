#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "pFuntion.h"



int main()
{

	/*Get current times*/
	struct tm *pt;
	time(&Current_Time);
	pt = localtime(&Current_Time);
		
	int NumberOne,NumberTwo,NumberThree,NumberFour,NumberFive;
	int x = 0;
	//int y = Max_Number;
	int k;
	
	struct NumberArray NA_Collect = {{7,9,21,22,28,33,34,38,1,5},
					{8,13,27,10,17,19,20,23,29,36},
					{2,3,4,11,14,16,24,25,26,35},
					{6,15,18,30,31,37,39,12,32}};

	int ItemOne[] = {7,9,21,22,28,33,34,38,1,5};
	int ItemTwo[] = {8,13,27,10,17,19,20,23,29,36};
	int ItemThree[] = {2,3,4,11,14,16,24,25,26,35};
	int ItemFour[] = {6,15,18,30,31,37,39,12,32};

	int Complete[Max_Number];
	
	/*Random Number Printf....................*/
	srand(time(NULL));

	NumberOne = (rand()%NELEMS(ItemOne));	
		Include_All(Complete,ItemOne[NumberOne],&x);
		
	NumberTwo = (rand()%NELEMS(ItemTwo));
		Include_All(Complete,ItemTwo[NumberTwo],&x);

	NumberThree = (rand()%NELEMS(ItemThree));
		Include_All(Complete,ItemThree[NumberThree],&x);
	
	NumberFour = (rand()%NELEMS(ItemFour));
		Include_All(Complete,ItemFour[NumberFour],&x);

	NumberFive = (rand()%39);
		Include_All(Complete,NumberFive,&x);

	Change_List(Complete);
	/*******************************************/


	/*Collect all number array to new numbers*/
	Collect_Array(NA_Collect.Item_One, NA_Collect.Item_Two, NA_Collect.Item_Three, NA_Collect.Item_Four,&x);


	for(k=0;k<Max_Number;k++)
	{
		printf("%d, ",Complete[k]);
	}
	printf("\n");
	switch(pt->tm_wday)
	{
		case 1:
			Days_Add(Complete,1);		
		break;
		case 2:
			Days_Add(Complete,2);
		break;
		case 3:
			Days_Add(Complete,3);
		break;
		case 4:
			Days_Add(Complete,4);
		break;

		case 5:
			Days_Add(Complete,5);
		break;

		case 6:
			Days_Add(Complete,6);
		break;

	}
	for(k=0;k<Max_Number;k++)
	{
		printf("%d, ",Complete[k]);
	}
	printf("\n");
	//printf("%d ",pt->tm_wday);

	return 0;
}

int Days_Add(int *Com,int Day)
{
	int i;
	for(i=0;i<Max_Number;i++)
	{
		*(Com+i) = *(Com+i)+Day;
		if(*(Com+i) > 39)
		{

			*(Com+i) = *(Com+i)-39;
		}
	}

	return 0;
}

/*collect all array*/
void Include_All(int *Com,int rNum,int *s)
{
	*(Com + *s) = rNum;
	*s+=1;
}

void Collect_Array(int *aCom, int* bCom, int* cCom, int* dCom,int *len)
{
	int tNumbers = 0;
	int CHANGENUMBER = 0;
	
	for(tNumbers = 0;tNumbers < 10;tNumbers++)
	{
		//printf("%d, ",*(aCom+tNumbers));
	}
	//printf("\n");
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
