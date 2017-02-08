#include "lib_file.h"

/***********Note***********/

In C ,functions can return any type except arrays and functions. We can get around this 
limitation by returning pointer to array or pointer to function.

/*************************/



/*********************#define printf(format)************************************/
//define another printf interface , use in format
#define PRINT(format...)	printf(format)

/********************************************************************************/

/****************************__attribute__((packed));;***************************/
//reduce space for use. Let system can work fast.
__attribute__((packed));;

//In normal --> sizeof : 8 bytes
typedef struct{
	int id;
	char arr[3];
}mode;

//if add _attribute__ ((packed))  --> sizeof : 7 bytes 
typedef struct                
{
    int id;
	char arr[3];
}__attribute__ ((packed)) mode;
/*****************************************************************************/




/************************char pointer array**********************************/
//when use char pointer array
char *array[3] = {"AnnexA","AnnexB","AnnexC"};

char string[32];

//pass array first value to string.
sprintf(string, "%s", array[0]);

//or array first value , the other method
sprintf(string, "%s", *array);

//array second value
sprintf(string, "%s", *(array+1));

/******************************************************************************/



/********************************Basic Enum*************************************/
//Basic enum use ,When define the one enum name ,the new enum can not used the same name.
typedef enum mode{
	Annex=0,
	AnnexB,
	AnnexC = 255,
	AnnexD = 256,
}mode;

int count = 0;

if(Annex == count)
	printf("Annex");
if(AnnexB != count)
	printf("count");
else
	the other
/***********************************************************************************/


/********************************Memory define & set*************************************/
//auto set memory size or can use it for men data to zero
char *mem;
memset(mem,0,size);


//dynamic set memory size,but after use needed to free the memory
char *mem;
mem = malloc(128);
.
.
.
.
free(mem);
/***********************************************************************************/

//char pointer string content
char *buf = "send"; --> it's really show "send\n\0"

//char array string content
char buf[] = "send"; --> it's mean "send\n"

/********************************strcmp function*************************************/
int ret = 0;
char *buf;
char *buf1

strcmp("hello","world")   -> return -1  : different
strcmp("abcde","ABCDE")   -> return 1   : the string was same,but case have different 
strcmp("abcde","abcde")   -> return 0   : The same
strcmp("12345","12345 ")  -> return -1  : The same , but the after have a space char.

NOTE : char pointer & char array in strcmp has return 1, because different size and char content.
/***********************************************************************************/

/*compare values for 1 or 0*/
int x = 0;
printf("%d\n", x ? 0 : 1);

ans -> 0;


/*define the variable,avoid the other files to repeat define it*/
extern int aa;
//It is need extra give the values.
aa = 0;



/**************************Funcion tmpnam() ****************************************/
//the tmpnam() function returns a pointer to a string that is a vaild filename,and such that a file with this name did not exist at some point in time, so that naive programmers may think it a suitable name for a temporary file.

//create a name for a temporary file in local/tmp -> include <stdio.h> 
char *tmpnam(char *s);

example : char buffer[L_tmpnam];
		  char *ptr;

		  tmpname(buffer);
		  printf("Temporary name 1 : %s\n",buffer);

		  ptr = tmpnam(NULL);
		  printf("Temporary name 2 : %s\n",ptr);

printf : Temporary name 1 : /tmp/filebaa1Tb
		 Temporary name 2 : /tmp/filedCIbb0

/*****************************************************************************************/



/********************** ntohs ntohl htons htonl ******************************************/
//all of this functions can use in change byte type to int type
example : //server
		  char *buffer; // if wants to get one data , but the value will over 255. 
		  (buffer[0] >> 8) & 0xff; // from server through the socket to client and to show message
		  buffer[1] & 0xff;

		  //client : Receive the data type was void pointer
		void *buf;
		int sum = 0;
			// get *buf values needed to trans the types to unsigned char* and before need to add pointer.
			// that is only way to get *buf value.
			sum = htonl(((*((unsigned char*)buf+0) & 0xff) << 8) | (*((unsigned char*)buf+1) & 0xff));


//ntons --> change network address to short type



//ntohl --> change network address to long type


//htons --> change host address to short type


//htonl --> change host address to long type



/*****************************************************************************************/


/*******************************Bitwise Complement****************************/

bitwise complement of N = ~N (represented in 2's complement form)
2'complement of ~N= -(~(~N)+1) = -(N+1)

printf("Number 1 : %d\n",~35);
printf("Number 2 : %d\n",~-12);

Output:
Number 1 : -36
Number 2 : 11


/****************************************************************************/

/******************************Reverse String********************************/
char str1[100];
char *fun(char str[])
{
	static int i = 0;

	if(*str)
	{
		fun(str+1);
		str1[i] = *str;
		i++;
	}
	return str1;
}
int main()
{
	char str[] = "abcdef123456";
	printf("%s\n", fun(str));
}
/****************************************************************************/

