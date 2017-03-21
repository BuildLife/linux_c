#include <stdio.h>
#include <stdlib.h>

typedef struct link{
	int val;
	struct link *next;
}linked;

void insert(linked *n1, linked *n2)
{
	n2 -> next = n1 -> next;
	n1 -> next = n2;
}

void RemoveNode(linked *n1)
{
	n1 -> next = n1 -> next -> next;
}

linked *create_node(int data)
{
	linked *n = (linked*)malloc(sizeof(linked));

	n -> val = data;
	n -> next = NULL;

	return n;
}


void free_allist(linked *lists)
{
	if(lists -> next != NULL)
	{
		free_allist(lists -> next);
	}
	free(lists);
}

void print_lists(linked *lists)
{
	linked *n = lists;

	while(n != NULL)
	{
		printf("%d ",n -> val);

		n = n -> next;
	}

	printf("\n");
}


int main()
{
	//defined node
	linked *lists = create_node(0);
	linked *b = create_node(5);
	linked *c = create_node(7);

	// 0 -> 5
	insert(lists, b);
	insert(lists, c);

	print_lists(lists);
	RemoveNode(c);
	print_lists(lists);
	free_allist(lists);

	return 0;
}


