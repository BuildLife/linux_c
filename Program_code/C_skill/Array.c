

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







