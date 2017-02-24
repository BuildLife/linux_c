/********************Defined***************/
#include<stdio.h>
#if X == 3      -->//In the first look. the output to be compile-time error because marco X has 
					//	not been defined. In C, if a marco is not defined, the pre-processor assigns 0 to it
					// by default.
	#define Y 3
#else 
	#define Y 5
#endif

int main()
{
	printf("%d",Y);
	return 0;
}

Answer : 5 //so the answer is 5 for printed.

/*****************************************************/

/******************** ********************************/



/******************** ********************************/










/******************** ********************************/









