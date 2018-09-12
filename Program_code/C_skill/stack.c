#include <stdio.h>
#include <stdlib.h>
#define MAX 10

typedef struct stack{
	int top;
	int value[MAX];
}stack;

//void pop(struct stack);
void push(struct stack*,int);
void empty_full(struct stack*);
void display(struct stack*);
int pop(struct stack*);
int main()
{
	int num = 0;
	int data = 0;
	stack *s = (struct stack*)malloc(sizeof(struct stack));

	while(1){
				printf("1. pop\n");
				printf("2. push\n");
				printf("3. display\n");
				printf("4. empty or full\n");
		scanf("%d",&num);
		switch(num)
		{
			case 1:
				pop(s);
			break;
		
			case 2:
				printf("enter data:\n");
				scanf("%d",&data);
				push(s,data);
			break;

			case 3:
				display(s);
			break;

			case 4:
	//			printf("empty or full\n");
				empty_full(s);	
			break;
		}

	}
	return 0;
}

void empty_full(struct stack *s)
{
	if(s->top == MAX)
	 	printf("this stack is full\n");
	else if(s->top == 0)
		printf("this stack is empty\n");
}

void push(struct stack *s,int data)
{
	if(s->top == MAX)
		printf("this stack is full\n");
	else
		s->value[s->top++] = data;
}

int pop(struct stack *s)
{
	struct stack *newnode;
	newnode = (stack*)malloc(sizeof(stack));
	
	newnode = s;
	newnode ->  

}

void display(struct stack *s)
{
	int i = 0;
	for(i = 0;i<s->top;i++)
		printf("%d,",s->value[i]);

}


