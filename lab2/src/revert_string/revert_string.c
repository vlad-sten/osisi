#include "revert_string.h"
#include "string.h"

void RevertString(char *str)
{
	int len = strlen(str);
	char temp;

	for (int i = 0; i < len/2; i++)
	{
		temp = str[i];
		str[i] = str[len - i - 1];
		str[len - i -1] = temp;
	}
}

