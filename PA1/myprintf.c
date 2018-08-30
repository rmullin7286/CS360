//2-1. Write YOUR own prints(char* s) function to print a string
void prints(char * s)
{
	while(*s != '\0')
	{
		putchar(*s);
		s++;
	}
}

typedef unsigned int u32;


void rpu(u32 x, int base)
{
	static const char * ctable = "0123456789DEF";
	char c;
	if(x)
	{
		c = ctable[x % base];
		rpu(x / base, base);
		putchar(c);
	}
}

//modified to make less code
void printu_base(u32 x, char * prefix, int base)
{
	prints(prefix);
	(x == 0) ? putchar('0') : rpu(x, base);
}

//2-2. Write YOUR OWN functions

void printd(int x)
{
	if(x < 0)
	{
		putchar('-');
		x = -x;
	}
	printu_base(x, "", 10);
}

//I decided to make all the functions macros so I didn't have to
//write as much
void printu(u32 x)
{
	printu_base(x, "", 10);
}

void printx(u32 x)
{
	printu_base(x, "0x", 16);
}

void printo(u32 x)
{
	printu_base(x, "0", 8);
}

//3. Write your own myprintf function
void myprintf(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	for(; *fmt != '\0'; fmt++)
	{
		if(*fmt == '%')
		{
			fmt++;
			switch(*fmt)
			{
				case 'c': putchar(va_arg(args, int));
					break;
				case 's': prints(va_arg(args, char *));
					break;
				case 'u': printu(va_arg(args, u32));
					break;
				case 'd': printd(va_arg(args, int));
					break;
				case 'o': printo(va_arg(args, u32));
					break;
				case 'x': printx(va_arg(args, u32));
					break;
				default:
					return;
			}
		}

		else if(*fmt == '\n')
		{
			prints("\r\n");
		}

		else
		{
			putchar(*fmt);
		}
		
	}
	va_end(args);
}