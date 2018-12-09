#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define NELEMS(x)  (sizeof(x)/(sizeof(x)[0]))

int lato_hot_ball[21] = {1,3,8,9,11,15,16,17,23,25,27,28,29,30,32,35,36,40,41,44,46};
int lato_cold_ball[7] = {4,5,24,26,34,38,48};

int g539_hot_ball[22] = {2,4,5,7,8,9,10,11,12,15,17,20,22,25,27,29,32,33,34,35,36,38};
int g539_cold_ball[7] = {3,13,14,23,24,37,39};


int f_FirstSecond(int *first, int *second, int len)
{
	int i = 0, j = 0;
	int x = 0, y = 0;
	int arr[30] = { 0 };

	for(i = 1;i< 40; i++)
	{
		if((*(first + x) - i) == 0)
		{
			arr[y] = i;
			y++;	
		}
		if((*(second + x) - i) == 0)
		{
			arr[y] = i;
			y++;
		}
		x++;
	}

	x = 0;
	for(x = 0;x<y;x++)
	{
		printf("%d,",arr[x]);
	}
	printf("\n");


	return TRUE;
}

int game_of_function(int game_num)
{
	int *gets_arr;
	int game_s = 0;
	int i = 0;

	gets_arr = (int *)malloc(10 * sizeof(int));
	if(game_num == 1)
	{
		game_s = f_539(gets_arr,5);
	}
	else if(game_num == 2)
	{
		game_s = f_lato(gets_arr,5);
	}
	else 
		exit(1);

	int tmp = 0,j = 0;

	printf("Game is %s\n",game_s == 5 ? "539" : "Lato");
	printf("Balls : ");
	for(i = 0; i < game_s; i++)
	{
		for(j = 0;j< game_s; j++)
		{	
			if(*(gets_arr + i) > *(gets_arr + j))
			{
				tmp = *(gets_arr + i);
				*(gets_arr + i) = *(gets_arr + j);
				*(gets_arr + j) = tmp;
			}
		}
	}
	
	for(j = 0;j<game_s;j++)
	{
		printf("%d,",gets_arr[j]);
	}

	printf("\n");
	free(gets_arr);
	return 0;
}

int f_lato(int *gets,int s)
{	
	int x = 0, y = 0;
	int ball_num = 0;
	int avoid_num[6] = {0};
	srand(time(NULL));
	for(x = 0;x<s-1;x++)
	{
		avoid_num[x] = lato_hot_ball[ball_num];
		ball_num = (rand()%NELEMS(lato_hot_ball));
		
		while(y <= x)
		{
			if(avoid_num[y] == lato_hot_ball[ball_num])
			{
				ball_num = (rand()%NELEMS(lato_hot_ball));
				y = 0;
			}
			y++;
		}	

		*(gets + x) = lato_hot_ball[ball_num];
		y = 0;
	}
		ball_num = rand()%NELEMS(lato_cold_ball);
		*(gets + (s - 1)) = lato_cold_ball[ball_num];
	return s;
}

int f_539(int *gets,int s)
{

	int x = 0, y = 0;
	int ball_num = 0;
	int avoid_num[5] = {0};
	srand(time(NULL));
	for(x = 0;x<s-1;x++)
	{
		avoid_num[x] = g539_hot_ball[ball_num];
		ball_num = rand()%NELEMS(g539_hot_ball);
		
		while(y <= x)
		{
			if(avoid_num[y] == g539_hot_ball[ball_num])
			{
				ball_num = (rand()%NELEMS(g539_hot_ball));
				y = 0;
			}
			y++;
		}	
		
		*(gets + x) = g539_hot_ball[ball_num];
		y = 0;
	}
		ball_num = rand()%NELEMS(g539_cold_ball);
		*(gets + (s - 1)) = g539_cold_ball[ball_num];
	return s;
}



int main()
{
	int ch_t = 0;
	int loop;
	int fir[5];
	int seco[5];
	
//	fir = (int *)malloc(10*sizeof(int));
//	seco = (int *)malloc(10*sizeof(int));

	while(1)
	{	
		printf("Options of Games:\n");
		printf("1. 539\n");
		printf("2. lato\n");
		printf("3. exit\n");

		scanf("%d",&ch_t);

		printf("Please input last week numbers\n");
		for(loop = 0; loop < 5; loop++)
			scanf("%d" ,&(fir[loop]));
		
		printf("Please input last before week numbers\n");
		for(loop = 0; loop < 5; loop++)
			scanf("%d" ,&(seco[loop]));
		
		f_FirstSecond(fir, seco, 5);
		
		game_of_function(ch_t);
	}

	return 0;
}
