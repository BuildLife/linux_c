#include "lib_file.h"


//define another printf interface , use in format
#define PRINT(format...)	printf(format)

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


//when use char pointer array
char *array[3] = {"AnnexA","AnnexB","AnnexC"};

char string[32];

//pass array first value to string.
sprintf(string, "%s", array[0]);

//or array first value , the other method
sprintf(string, "%s", *array);

//array second value
sprintf(string, "%s", *(array+1));



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


//char pointer string content
char *buf = "send"; --> it's really show "send\n\0"

//char array string content
char buf[] = "send"; --> it's mean "send\n"


//strcmp function
int ret = 0;
char *buf;
char *buf1

strcmp("hello","world")   -> return -1  : different
strcmp("abcde","ABCDE")   -> return 1   : the string was same,but case have different 
strcmp("abcde","abcde")   -> return 0   : The same
strcmp("12345","12345 ")  -> return -1  : The same , but the after have a space char.

NOTE : char pointer & char array in strcmp has return 1, because different size and char content.


/*compare array to 1 or 0*/
int x = 0;
printf("%d\n", x ? 0 : 1);

ans -> 0;





