/*************** Pointer assign **********************/

int a;
char *x = (char*)&a;

a = 512;   --> // bit :   0000 0010 0000 0000
// if pass to char *x -->    x[1]     x[0] 

x[0] = 1; //set value to x[0]
x[1] = 2; //set value to x[1]

printf("%d\n",a);

//                2         1
// a -> 512 : 0000 0010 0000 0001
//x[0] = 1 -> becomes 1. 0000 0000 -> 0000 0001
//x[1] = 2 -> becomes 2. 0000 0010 -> 0000 0010 : value still 2


Answer : 513 //so bit : 0000 0010 0000 0001

/**********************************************************/

/*********************Use Pointer change variable**********/

void change(int *a, int *b)
{
	if(*a != *b)
	{
		*a = *a ^ *b;
		*b = *a ^ *b;
		*a = *a ^ *b;
	}
}

int main()
{
	int x = 50;
	int y = 100;

	change(&x,&y);

	printf("x = %d,y = %d\n".x,y);

	return 0;
}

Answer : x = 100, y = 50;

/************************************************************/









