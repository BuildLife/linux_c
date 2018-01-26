#include "lib_file.h"



typedef struct _employee{
	char name[32];
	unsigned char age;
}Employee;

void init_List(LinkedList *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->current = NULL;
}

void addHead(LinkedList *list, void *data)
{
	Node *node = (Node*)malloc(sizeof(Node));
	node -> data = data;
	if(list -> head == NULL)
	{
		list -> tail = node;
		node -> next = NULL;
	}
	else
	{
		node -> next = list -> tail;
	}
		list -> head = node;
}

void addTail(LinkedList *list, void *data)
{
	Node *node = (Node*)malloc(sizeof(Node));
	node -> data = data;
	node -> next = NULL;

	if(list -> head == NULL)
	{
		list -> head = node;
	}
	else
	{
		list -> tail -> next = node;
	}
	list -> tail = node;
}

void dis(Employee *emp)
{
		printf("name = %s\n",emp->name);
		printf("age = %d\n",emp->age);
}

void display(LinkedList *list)
{
	Node *node = list -> head;
	while(node != NULL)
	{
		dis(node->data);
		node = node -> next;
	}
}



int main()
{
	LinkedList linkedlist;

	Employee *samuel = (Employee*)malloc(sizeof(Employee));
	strcpy(samuel -> name, "Samuel");
	samuel -> age = 66;

	Employee *sally = (Employee*)malloc(sizeof(Employee));
	strcpy(sally -> name, "Sally");
	sally -> age = 81;

	init_List(&linkedlist);

	addTail(&linkedlist, samuel);
	addTail(&linkedlist, sally);

	display(&linkedlist);

	return 0;
}
