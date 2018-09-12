#include <stdio.h>
#include <stdlib.h>

struct node
{
	int key;
	struct node *left,*right;
};

int main()
{
	struct node *tt = (struct node*)malloc(sizeof(struct node));
	tt->key = 99;
	printf("%d\n",tt->key);
	free(tt);
	return 0;
}
