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





