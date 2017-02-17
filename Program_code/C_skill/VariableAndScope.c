/*********extern*******************************/

extern int var = 0; 

//Can not to be use
extern int var; --> //var isn't allocated any memory.
					//And the program is trying to change the value to 10 of a variable 
					//that doesn't exist at all.

int main()
{
	var = 10;
	printf("%d",var);
	return 0;
}
/*********************************************/

/**************** Defined variable ************************/

int var = 032;  <-- //It's considered as octal number.
printf("%d\n",var) --> //Therefore the value of var is  3 * 8 + 2 = 26

/********************************************************/







