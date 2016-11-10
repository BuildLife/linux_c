#include "lib_file.h"

int ConvertToInt(char *str)
{
	int i=0;
	int istr = 0;
	int nat = 1;

	if(str[0] == '-')
	{
		nat = -1;
		i++;
	}

	for(;str[i] != '\0';++i)
	{
		istr = istr * 10 + str[i] - '0';
	}
	return nat * istr;
}

/*int main(int argc,char **argv)
{
	if(argc < 2)
	{
		printf("Usage : ./T_atoi 1234");
		return;
	}
	int istr;
	istr = ConvertToInt(argv[1]);
	printf("Convert string to int : %d\n",istr);



	return 0;
}*/
