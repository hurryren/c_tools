int short2hex(unsigned short s, unsigned char *p);
unsigned short hex2short(unsigned char *p);
unsigned int dec2int(unsigned char *buffer, int len);
unsigned int hex2int(char *buffer, int len);
int uint2hex( unsigned int i, unsigned char *p );
unsigned int hex2uint(unsigned char *p);
unsigned int crc32(unsigned char *buf, int len);

unsigned long GetMsgType();
int CheckNum(char *str);
int num2int(unsigned char *buffer, int len);

int PackBCD( char *InBuf, unsigned char *OutBuf, int Len );
int UnpackBCD( unsigned char *InBuf,  char *OutBuf, int Len );

int GetKeyLengthByScheme( char keyscheme );
int GetKeyLengthFromBuffer( char *buffer );
int GetKeyBCDValue( char *key, unsigned char *uckey, int *uckeylen );
int CheckDERPublicKeyLength( unsigned char *pubkey_der );

int getPasswd( char *pwd );

