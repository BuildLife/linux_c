#include <stdio.h>
#include <stdlib.h>
#define MAXVALUE 10

typedef struct queue{
	int front;
	int rear;
	int value[MAXVALUE];
}queue;

void init_queue(struct queue*);
void push_queue(struct queue*, int data);
void pop_queue(struct queue*);
int empty(struct queue*);
int full(struct queue*);



int main()
{
	int num = 0;
	int data_push = 0;
	struct queue* qu;
	qu = (struct queue*)malloc(sizeof(struct queue));
	init_queue(qu);
	while(1)
	{
		printf("1.push\n");
		printf("2.pop\n");
		printf("3.display\n");
		printf("4.empty or full\n");
		scanf("%d",&num);
		switch(num)
		{
			case 1:
				printf("Push you want data:\n");
				scanf("%d",&data_push);
				push_queue(qu,data_push);
			break;

			case 2:
				pop_queue(qu);
			break;

			case 3:
				display(qu);
			break;
			case 4:
			{
				int x = 0;
				for(x = 0;x<MAXVALUE;x++)
				printf("%d,",qu->value[x]);

				printf("\n");
			}
			break;

		}
	}	
	return 0;
}

int full(struct queue* fu_qu)
{
	return fu_qu -> rear == MAXVALUE && fu_qu -> front == 0;
}

int empty(struct queue *em_qu)
{
	return em_qu -> front == em_qu -> rear;
}


void init_queue(struct queue* in_qu)
{
	in_qu -> front = 0;
	in_qu -> rear = 0;
	//in_qu -> value[MAXVALUE] = {0};
	//memcpy(in_qu->value, 0 ,sizeof(in_qu->value));
}

void push_queue(struct queue* ad_qu, int data)
{
	if(full(ad_qu))
	{
		printf("This queue have been full\n");
	}
	else
	{
		ad_qu -> value[ad_qu->rear++] = data;
	}
}

void pop_queue(struct queue* pop_qu)
{
	int i = 0;
	if(empty(pop_qu))
	{
		printf("This queue has empty\n");
		pop_qu -> rear = 1;
		pop_qu -> front = 1;
	}
	else
	{
		pop_qu -> value[pop_qu->front++] = NULL;
		/*i = pop_qu -> rear;
		while(pop_qu -> value[pop_qu -> front] != '\n')
		{
			pop_qu -> value[pop_qu->front] = pop_qu -> value[i];
		}*/
	}
}

void display(struct queue* dis_qu)
{
	int i = 0;
	for(i = dis_qu->front;i<dis_qu -> rear;i++)
	{
		printf("%d,",dis_qu -> value[i]);
	}
	printf("\n");
}
