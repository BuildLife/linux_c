#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define NELEMS(x)  (sizeof(x)/(sizeof(x)[0]))

int lato_hot_ball[21] = {1,3,8,9,11,15,16,17,23,25,27,28,29,30,32,35,36,40,41,44,46};
int lato_cold_ball[7] = {4,5,24,26,34,38,48};

int g539_hot_ball[22] = {2,4,5,7,8,9,10,11,12,15,17,20,22,25,27,29,32,33,34,35,36,38};
int g539_cold_ball[7] = {3,13,14,23,24,37,39};


int game_of_function(int game_num)
{
	int *gets_arr;
	int game_s = 0;
	int i = 0;

	gets_arr = (int *)malloc(10 * sizeof(int));
	if(game_num == 1)
	{
		game_s = 5;
		f_539(gets_arr,game_s);
	}
	else if(game_num == 2)
	{
		game_s = 6;
		lato_f(gets_arr,game_s);
	}
	else 
		exit(1);


	printf("Game is %s\n",game_s == 5 ? "539" : "Lato");
	printf("Balls : ");
	for(i = 0; i < game_s; i++)
	{
		printf("%d,",gets_arr[i]);
	}

	printf("\n");
	free(gets_arr);
	return 0;
}

int lato_f(int *gets,int s)
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
	return 0;
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
	return 0;
}



int main()
{
	int ch_t = 0;

	while(1)
	{	
		printf("Options of Games:\n");
		printf("1. 539\n");
		printf("2. lato\n");
		printf("3. exit\n");
		scanf("%d",&ch_t);
		game_of_function(ch_t);
	}
	return 0;
}
