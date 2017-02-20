

/*********************Insert string to Array*******************************/

unsigned char var[100][6] = {0}; -->//Simple to understand is 100 items and six character.

int i = 0; --> // First Item.

char x_ch[6] = {0x61,0x62,0x63,0x64,0x65,0x66}; -->//Defined variable for ready to pass array.

strcpy(var[i], x_ch); -->//Mean x_ch variable copy to first item of var arrays.

for(int j = 0; j < 6; j++)
{
	printf("%c", var[0][j]); --> // Show the first items of var arrays.
}
printf("\n");

The Show Answer is : a,b,c,d,e,f,


/***************************************************************************/



/****************** Basic Array calculation **********************************/

int arr[5]; //Assume that base address of arr is 2000 and size of integer is 32bit

arr++;   ---->> //ERROR : Array name in C is implemented by a constant pointer. It is not possible to apply increment 
				//			and decrement on constant types.
printf("%u",arr);


---> correct 
int arr[5];
printf("%u %u", arr+1, &arr+1);

Answer : 2004 2020   // Name of array in C gives the address(expect in sizeof operator) of the first element.
					//	Adding 1 to this address gives the address plus the sizeof type the array has.
					// Applying the address-of operator before the array name gives the address of the who-
					//	-le array. Adding 1 to this address gives the address plus the sizeof whole array.

/*********************************************************************************/


/********************************Size of pass array to function ***********************************/
void print(int arr[]) // if pass for function array, it needed also to pass array size.
{
	int n = sizeof(arr)/sizeof(arr[0]);  --> // 32bit : sizeof(arr) -> 4  ; 64bit : sizeof(arr) -> 8
											// sizeof(arr)/sizeof(arr[0]) becomes sizeof(int *)/sizeof(int)
	int i;
	for(i = 0;i<n;i++)
		printf("%d",arr[i]);
}
int main()
{
	int arr[] = {1,2,3,4,5};
	print(arr);
	return 0;
}

Answer : 1 

/***************************************************************************************************/



/******************************** Array addres plus 1***********************************/

int main()
{
	int as[] = {1,2,3,4,5,6};
	int *ptr = (int *)(&as+1);  --> // It's gets 'as' array all values 1 ~ 6 ,and also gets next value 7;
	printf("%d", *(ptr-1));  --> // It's show ,that address 7 div 1 . So back to the address 6.
								//But if *(ptr) for show , it's overflow .
	return 0;
}

Answer : 6

/***************************************************************************************************/


/*************************** Pointer Array ***************************************************/
int *A[10];
int C[] = {12,13,14,15};

A[2] = C;

Answer : A[2][3] = 15;  A[2][0] = 12;

A[0] = C;

Answer : A[0][3] = 15; A[0][0] = 12;



/***************************************************************************************************/



/**************************  Array ***************************************************/

For every type T expect void and function type, there can be an array of T.


//For Pointer aray
int main()
{
	void arr[100] --> //It's compiler error
}

int main()
{
	void *arr[100] --> //We can have array of void pointers and function pointers.
}

//Function pointer in c 

void fun(int a)
{
	printf("Value of a is %d\n",a);
}

int main()
{
	void (*fun_ptr)(int) = &fun; -->//fun_ptr is pointer to function fun();
	
	/*
		The above line is equivalent of following two
		void (*fun_ptr)(int);
		fun_ptr = &fun;
	*/


	(*fun_ptr)(10); --> //Invoking fun() using fun_ptr
}

Answer : Value of 1 is 10

/***************************************************************************************************/
