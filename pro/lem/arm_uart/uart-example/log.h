#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

char *GetTime( char *Buffer, int Len, const char *format )
{
	time_t        clock;

	clock = time((time_t *)0);
	strftime (Buffer,Len,format,localtime(&clock));

	return (Buffer);
}

void debuglog(const char *func, const char *format, ...)
{
	FILE *fp;
	char buffer[2048];
	char Date[9+2],Time[9];
	char logfilename[] = "";

	fp = fopen("/tmp/cl_mnger.log","a+");
	if (!fp)
		return;

	va_list Args;
	va_start(Args,format);
	vsprintf(buffer,format,Args);
	va_end(Args);

	GetTime(Date,9+2,"%Y-%m-%d");
	GetTime(Time,9,"%H:%M:%S");

	fprintf(fp,"<%s %s> [%d] %s: %s\n",Date,Time,getpid(),func,buffer);
	fclose(fp);	
}



