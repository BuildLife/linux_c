#include "lib_file.h"
#include <stdint.h>


uint32_t htonf(float f)
{
	uint32_t p;
	uint32_t sign;

	if(f<0)
	{
		sign = 1;
		f = -f;
	}
	else
		sign = 0;

	p = ((((uint32_t)f)&0x7fff) << 16) | (sign << 31); //whole part and sign

	p |= (uint32_t)(((f - (int)f) * 65536.0f))&0xffff; //fraction

	return p;
}

float ntohf(uint32_t p)
{
	float f = ((p >> 16)&0x7fff);
	f += (p&0xffff) / 65536.0f;

	if(((p>>31)&0x1) == 0x1)
	{
		f = -f;
	}

	return f;
}

int main(int argc,char **argv)
{
	if(argc < 2)
	{
		printf("Usage : ./encode_test_S 3.151\n");
		return;
	}
	float change;
	change = atof(argv[1]);

	uint32_t getn;
	float testf;
	getn = htonf(change);
	printf("%u\n",getn);

	testf = ntohf(getn);
	printf("%f\n",testf);


	return 0;
}
