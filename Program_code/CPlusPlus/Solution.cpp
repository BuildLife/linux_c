#include "BasicLib.h"



int Solution::Fstrstr(std::string haystack, std::string needle)
{
	int ret = haystack.find(needle, 0);
	if(ret >= 0)
		return ret;

	return -1;
}
