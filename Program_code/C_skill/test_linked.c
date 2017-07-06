#include<stdio.h>
#include <stdlib.h>


typedef struct node{
	int data;
	struct node *next;
}NODE;


void insert(NODE *,int );
void removed(NODE *,int );
void display(NODE *);

int main()
{
	NODE *list;
	printf("size : %d\n",sizeof(struct node));
	list = (struct node*)malloc(sizeof(struct node));
	list -> data = 0;
	list -> next = NULL;
	int number = 0;
	int in_num = 0;
	int re_num = 0;

	while(1)
	{
		printf("1. insert\n");
		printf("2. display\n");
		printf("3. remove \n");
		printf(":");
		scanf("%d",&number);
		switch(number)
		{
			case 1:
					printf("insert : ");
					scanf("%d",&in_num);
					printf("insert : %d\n",in_num);
					insert(list,in_num);
			break;

			case 2:
					printf("display :");
					display(list);
			break;
			
			case 3:
				printf("Enter remove number :\n");
					scanf("%d",&re_num);
					printf("remove : %d\n",re_num);
					removed(list,re_num);
				break;
		}

	}


	return 0;
}


void insert(NODE *list, int sert)
{
	NODE *temp;
	temp = (struct node*)malloc(sizeof(struct node));
	if(list -> data == 0)
	{
		list -> data = sert;
		list -> next = NULL;
	}
	else
	{
		while(list -> next != NULL)
			list = list -> next;

		list -> next = (struct node*)malloc(sizeof(struct node));
		
		list -> next -> data = sert;
		list -> next -> next = NULL;
	}
}
void removed(NODE *list, int rem)
{
	NODE *temp;
	temp = (struct node*)malloc(sizeof(struct node));
	if(list -> data == rem)
	{
		temp = list -> next;
		free(list);
		list = temp;
	}
	else
	{
		while(list -> next -> data != rem)
			list = list -> next;

		temp = list -> next -> next;
		free(list -> next);
		list -> next = temp;
	}
}

void display(NODE *list)
{
	NODE *no = list;
	while(no)
	{
		printf("%d,",no -> data);
		no = no -> next;
	}
	printf("\n");
}
