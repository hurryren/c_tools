#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef _WIN32                   //Linux platform
#include <termio.h>
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#include "util_tools.h"

#define MAX_CHARATER_LEN   1024

int getch(void)
{
        struct termios tm, tm_old;
        int fd = STDIN_FILENO, c;
        if(tcgetattr(fd, &tm) < 0)
                return -1;
        tm_old = tm;
        cfmakeraw(&tm);

        tm.c_cc[ VMIN ]  = 1;
        tm.c_cc[ VTIME ] = 0;

        if(tcsetattr(fd, TCSANOW, &tm) < 0)
                return -1;
        c = fgetc(stdin);
        if(tcsetattr(fd, TCSANOW, &tm_old) < 0)
                return -1;
        return c;
}

#else                            //WIN32 platform
#include <conio.h>
#endif

void printbuf(char *title, unsigned char *buffer, int len)
{
	int i;

	printf("%s[%d]\n", title, len);
	for(i=0;i<len;i++)
	{
		printf("%02X ", buffer[i]);
		if (i%16==15)
			printf("\n");
	}
	printf("\n");
}


int getPasswd( char *pwd )
{
    int i=0, ch;
    static char p[MAX_CHARATER_LEN+1]="";
    
    while((ch = getch())!= -1 && ch != 13)
    {
         if(i == MAX_CHARATER_LEN && ch != 8)
         {
              putchar(7);
              continue;
         }
         if(ch == 8)
         {
             if(i==0)
              {
                  putchar(7);
                  continue;
              }
              i--;
              putchar(8);
              putchar(' ');
              putchar(8);
         }
         else
         {
             p[i] = ch;
             putchar('*');
             i++;
         }
    }

    if(ch == -1)
    {
        while(i != -1)
        {
            p[i--] = '\0';
        }
        return -1;
    }
    p[i]='\0';
    printf("\n");
    
    strcpy( pwd, p );
    return 0;
}


int short2hex( unsigned short s, unsigned char *p )
{
	p[0] = (s>>8)&0xff;
	p[1] = s&0xff;

	return 0;
}

unsigned short hex2short(unsigned char *p)
{
	unsigned short s;
	s = p[0];
	s=(s<<8) | p[1];

	return s;
}

int uint2hex( unsigned int i, unsigned char *p )
{
	*p++=(i>>24)&0xff;
	*p++=(i>>16)&0xff;
	*p++=(i>>8)&0xff;
	*p++=i&0xff;

	return 0;
}

unsigned int hex2uint(unsigned char *p)
{
	unsigned int i;

	i=*p++;
	i=(i<<8)|*p++;
	i=(i<<8)|*p++;
	i=(i<<8)|*p++;

	return i;
}

/*** Convert decimal char's buffer to integer  */
unsigned int dec2int(unsigned char *buffer, int len)
{
        register unsigned int  rv = 0, i;
        register unsigned char *ptr;

        for(ptr=buffer, i=0;i<len;i++,ptr++)
        {
                rv = rv*10 + *ptr-'0';
        }
        return rv;
}

unsigned int hex2int(char *buffer, int len)
{
	register unsigned int  rv = 0, i;
	register char *ptr;

	for(ptr=buffer, i=0;i<len;i++,ptr++)	
	{
		rv = rv*16 + (((*ptr=toupper(*ptr))>'9')?(*ptr-'A'+10):(*ptr-'0'));
	}
	return rv;
}

int CheckNum(char *str)
{
	int	i;
	
	for(i=0;i<strlen(str);i++){

		if ( ( str[i] < '0' ) || ( str[i] > '9' ) ) return(-1);
	}
	
	return(0);
}

int num2int(unsigned char *buffer, int len)
{
	register int  rv = 0, i;
	register unsigned char *ptr;
	
	for(ptr=buffer, i=0;i<len;i++,ptr++)
	{
		if ( toupper(*ptr) > '9' )
			return -1;
		
		rv = rv*10 + (*ptr-'0');
	}
	return rv;
}

unsigned long GetMsgType()
{
	int    mtype;

//    mtype =  ( getpid() * 1000L + pthread_self() ) % 0x7fffffff;	// + thread id
    mtype =  getpid();// V1.21.003	

    return (long)mtype;
}

unsigned int crc32(unsigned char *buf, int len)
{
	int i, j;
	unsigned int crc, mask;

	crc = 0xFFFFFFFF;
	for (i = 0; i < len; i++) 
	{
		crc = crc ^ (unsigned int)buf[i];
		for (j = 7; j >= 0; j--) 
		{    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}
	return ~crc;
}

int GetKeyLengthByScheme( char keyscheme )
{
	if ( keyscheme == 'Z' )
		return 8;
	
	if ( keyscheme == 'X' )
		return 16;
	
	if ( keyscheme == 'Y' )
		return 24;
	
	return -1;
}

int GetKeyLengthFromBuffer( char *buffer )
{
	if ( buffer[0] == 'X' )
		return 32+1;
	if ( buffer[0] == 'Y' )
		return 48+1;
	
	return 16;
}

int GetKeyBCDValue( char *key, unsigned char *uckey, int *uckeylen )
{
	int rv;
	
	if ( key[0] == 'X' )
	{
		if ( strlen(key) < 32+1 )
			return -1;
			
		rv = PackBCD( &key[1], uckey, 32 );
		*uckeylen = 16;
	}
	else if ( key[0] == 'Y' )
	{
		if ( strlen(key) < 48+1 )
			return -1;
			
		rv = PackBCD( &key[1], uckey, 48 );
		*uckeylen = 24;
	}
	else
	{
		if ( strlen(key) < 16 )
			return -1;
			
		rv = PackBCD( key, uckey, 16 );
		*uckeylen = strlen(key) / 2;
	}
		
	return rv;
}

int UnpackBCD( unsigned char *InBuf,  char *OutBuf, int Len )
{

	int rc = 0;

	unsigned char ch;

	register int i, active = 0;

	for ( i = 0; i < Len; i++ )
	{

		ch = *InBuf;

		if ( active )
		{
			(*OutBuf=(ch&0xF))<10 ? (*OutBuf += '0') : (*OutBuf += ('A'-10));
			InBuf++;
		}
		else
		{
			(*OutBuf = (ch&0xF0)>>4)<10 ? (*OutBuf += '0') : (*OutBuf+=('A'-10));
		}

		active ^= 1;

		if ( !isxdigit ( *OutBuf ) )	/* validate character */
		{
			rc = -1;
			break;
		}

		OutBuf++;

	}

	*OutBuf = 0;

	return ( rc );

}

// string:"0592389FFFEDCBA9" -> X'05 X'92 X'38 X'9F X'FF X'ED X'CB X'A9, 8bytes unsigned char []
int PackBCD( char *InBuf, unsigned char *OutBuf, int Len )
{
	int	    rc;		/* Return Value */

	register int     ActiveNibble;	/* Active Nibble Flag */

	char     CharIn;	/* Character from source buffer */
	unsigned char   CharOut;	/* Character from target buffer */

	rc = 0;		/* Assume everything OK. */

	ActiveNibble = 0;	/* Set Most Sign Nibble (MSN) */
				/* to Active Nibble. */

	for ( ; (Len > 0); Len--, InBuf++ )
	{
		CharIn = *InBuf;
		
		if ( !isxdigit ( CharIn ) )	/* validate character */
		{
			rc = -1;
		}
		else
		{
			if ( CharIn > '9')
			{
				CharIn += 9;	/* Adjust Nibble for A-F */
			}
		}

		if ( rc == 0 )
		{

			CharOut = *OutBuf;
			if ( ActiveNibble )		
			{
				*OutBuf++ = (unsigned char)( ( CharOut & 0xF0) |
					     ( CharIn  & 0x0F)   );
			}
			else
			{
				*OutBuf = (unsigned char)( ( CharOut & 0x0F)   |
					   ( (CharIn & 0x0F) << 4)   );
			}
			ActiveNibble ^= 1;	/* Change Active Nibble */
		}
	}

	return rc;

}

int CheckDERPublicKeyLength( unsigned char *pubkey_der )
{
	unsigned short derhead;
	unsigned int derlen = 0;
	
	if ( !pubkey_der )
		return 0;

	derhead = pubkey_der[0] << 8 | pubkey_der[1];	
	if ( derhead == 0x3081 )
	{
		derlen = pubkey_der[2];
		
		return derlen + 3;
	}
	else if ( derhead == 0x3082 )
	{
		derlen = hex2short( &pubkey_der[2] );

		return derlen + 4;
	}
	else
		return -1;
}

