#include "myprintf.h"

int main(void)
{
	char c = 'c';
	char * s = "test string";
	int d = -50;
	unsigned int u = 50;
	
	myprintf("c=%c, s=%s, d=%d, u=%u, o=%o, u=%u", c, s, d, u, d, d);
}