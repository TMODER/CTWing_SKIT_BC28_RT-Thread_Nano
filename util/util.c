#include <stdint.h>
#include <string.h>

#include "util.h"

bool endianMode = LITTLE_ENDIAN;   


uint32_t htonl(uint32_t a);
uint32_t ntohl(uint32_t a);


/****************************************************************/

/*judge high or low byte*/
bool isLittleEndian(void) 
{
	BYTE_HIGH_LOW byte;
	
	byte.number = 0x01000002;
  endianMode = byte.s == 0x01 ? BIG_ENDIAN:LITTLE_ENDIAN;
	
	return endianMode;
}

/******************************************************************/

void htonf_hexstr1(float data, char *dst) 
{        
    FLOAT_UNION temp2;
    temp2.value = data;
    
    uint8_t motorStatusArray[4];
		
    if(endianMode == LITTLE_ENDIAN)  
    {
        motorStatusArray[0] = temp2.float_byte.high_byte;
        motorStatusArray[1] = temp2.float_byte.mhigh_byte;
        motorStatusArray[2] = temp2.float_byte.mlow_byte;
        motorStatusArray[3] = temp2.float_byte.low_byte;
    }
    else
    {
        motorStatusArray[3] = temp2.float_byte.high_byte;
        motorStatusArray[2] = temp2.float_byte.mhigh_byte;
        motorStatusArray[1] = temp2.float_byte.mlow_byte;
        motorStatusArray[0] = temp2.float_byte.low_byte;
    }
  
		bytes_to_hex((char*)motorStatusArray, (char*)dst, 4);
}

typedef union
{
	float    float_data;
	uint32_t int_data;
	uint8_t  byte_data[4];
}mixture_4bytes_t;

void htonf_hexstr2(float data, char *dst) 
{        
    mixture_4bytes_t mixture_data;
	
	  mixture_data.float_data = data;
    
    if(endianMode == LITTLE_ENDIAN)  
    {
			mixture_data.int_data  = htonl(mixture_data.int_data);
    }
    
		bytes_to_hex((char*)mixture_data.byte_data, (char*)dst, 4);
}

void htonf_hexstr3(float data, char *dst) 
{        
    uint8_t temp[4];
		uint8_t *mixture = (uint8_t *)&data;
	  
	  if(endianMode == LITTLE_ENDIAN)
		{
			temp[0] = mixture[3];//MSB
			temp[1] = mixture[2];
			temp[2] = mixture[1];
			temp[3] = mixture[0];//LSB
		}
		else
    {
			temp[0] = mixture[0];//MSB
			temp[1] = mixture[1];
			temp[2] = mixture[2];
			temp[3] = mixture[3];//LSB
		}
			
		bytes_to_hex((char*)temp, (char*)dst, 4);
}


/***************************************************************/

static uint8_t find_targe(char data)
{
	char targe[10] = {',','.',' ','\0'};
	uint8_t cnt=0;
	
	while(targe[cnt] != '\0')
  {
		if( targe[cnt] == data )
		{
			return cnt;
		}
		cnt++;
	}
	
	
	return 0xff;
}

//go out of use
char m_strtok(char *in, char **args, uint8_t *argc)
{
	char *p=in;
	char flag=0;
	
	if( in == NULL || args ==NULL || argc == NULL ) 
		return 0;
	
	*argc = 0;

	while(*p != '\0')
	{
		if( find_targe(*p) != 0xff )  //,x  ,, 
		{
			*p = '\0';
      flag = 0;
		}
		else
		{
			if( flag == 0 )
			{
				flag = 1;
				*args++ = p;
				(*argc)++;
			}
		}
		p++;
	}
	
	return 1;
}

/*********************************************************************/

static uint8_t find_use_targe(char data, char *targe)
{
	uint8_t cnt=0;
	
	if( targe == NULL || *targe == '\0' )
		return 0xff;
	
	while(targe[cnt] != '\0')
  {
		if( targe[cnt] == data )
		{
			return cnt;
		}
		cnt++;
	}
	
	
	return 0xff;
}

char t_strtok(char *in, char *targe, char **args, uint8_t *argc)
{
	char *p=in;
	char flag=0;
	
	if( in == NULL || *in == '\0' || args ==NULL || argc == NULL || targe == NULL || *targe == '\0' ) 
		return 0;
	
	/**********parse in params************/
	*argc = 0;

	while( *p != '\0' )
	{
		if( find_use_targe(*p, targe) != 0xff )  //,x  ,, 
		{
			*p = '\0';
			flag = 0;
		}
		else
		{
			if( flag == 0 )
			{
				flag = 1;
				*args++ = p;
				(*argc)++;
			}
		}
		p++;
	}
	/*************end param ***************/
	
	return 1;
}

char t_strtok2(char *in, char *targe, char **args, uint8_t *argc)
{
	char *p=in;
	
	if( in == NULL ) return 0;
	if( args ==NULL ) return 0;
	
	
	/**********parse in params************/
	*argc = 1;
	*args++ = p;
	while(*p != '\0')
	{
		if( find_use_targe(*p, targe) != 0xff )  //,x  ,, 
		{
			if( find_use_targe(*(p+1), targe) == 0xff )
			{
				*p = '\0';
				*args++ = ++p;
				(*argc)++;
			}
			else
      {
				*p++ = '\0';
				//p++;
				*args++ = '\0';
				(*argc)++;
			}
		}
		else
		{
			p++;
		}
	}
	/*************end param ***************/
	
	return 1;
}

/*************************************************************************
* @parmas:
         in :   
         targe:
         args:
         argc:
* note:This function is used to filter continuous multi-character targets

explame: 
in: OK\r\n\r\nOK\r\n6

targe: \r\n

argc: 3

args[0]: OK
args[1]: OK
args[2]: 6
**************************************************************************/

 int8_t n_strtok(char *in, char *targe, char **args, uint8_t *argc)
{
	char *p=in;
  int targe_len;
	int in_len;
	int frist=0;
	
	if( in == NULL || *in == '\0' || args ==NULL || argc == NULL || targe == NULL || *targe == '\0' ) 
		return -1;

	targe_len = strlen(targe);
	in_len    = strlen(in);
	
	//clear "in" string by "targe" string 
	while( (p = strstr(p, targe)) != NULL )
	{
		memset(p, 0, targe_len);
			
		p += targe_len;
		
		if( *p == '\0' )
			break;
	}	
	
	//recode args and argc
  *argc = 0;
	p = in;
	
	while( p < in + in_len )
	{
		if( *p != 0 )
    {
			if( frist == 0 )
			{
				frist = 1;
				*args++ = p;
				(*argc)++;
			}		
		}
		else
		{
			frist = 0;
		}
		
		p++;
	}
	/*************end param ***************/
	
	return 0;
}

/*************************************************************************
* @functions: shell_split
* @parmas:
							 in :   
							 targe:
							 args:
							 argc:
* note:This function is used to filter continuous multi-character targets

explame: 
in: OK\r\n\r\nOK\r\n6

targe: \r\n

argc: 3

args[0]: OK
args[1]: OK
args[2]: 6
**************************************************************************/

 int8_t shell_split(char *in, char *targe, char **args, uint8_t *argc)
{
	char *p=in;
  int targe_len;
	int in_len;
	int frist_substr=0;
	int argc_max = *argc;
	
	if( in == NULL || *in == '\0' || args ==NULL || argc == NULL || targe == NULL || *targe == '\0' ) 
		return -1;

	targe_len = strlen(targe);
	in_len    = strlen(in);
	
	//clear "in" string by "targe" string 
	while( (p = strstr(p, targe)) != NULL )
	{
		memset(p, 0, targe_len);
			
		p += targe_len;
		
		if( *p == '\0' )
			break;
	}	
	
	//recode args and argc
  *argc = 0;
	p = in;
	
	while( p < in + in_len )
	{
		if( *p != 0 )
    {
			if( frist_substr == 0 )
			{
				frist_substr = 1;
				*args++ = p;
				(*argc)++;
				
				if( *argc == argc_max )
					return 0;
			}		
		}
		else
		{
			frist_substr = 0;
		}
		
		p++;
	}
	/*************end param ***************/
	
	return 0;
}

/*********************************************************************/

uint32_t bytes_to_hex(const char *pSrc, char *pDst, uint32_t nSrcLength)
{
	  if( pSrc == NULL || pDst == NULL )
			return 0;
		
    for (uint32_t i = 0; i < nSrcLength; i += 1)
    {
        int n1 = *pSrc >> 4;
        int n2 = *pSrc & 0XF;
        *pDst = (n1 <= 9) ? n1 + '0' : n1 - 10 + 'a';
        pDst++;
        *pDst = (n2 <= 9) ? n2 + '0' : n2 - 10 + 'a';

        pSrc++;
        pDst++;
    }

    return nSrcLength / 2;
}


static uint8_t ACII2Byte(uint8_t ACII)
{
	uint8_t result = 0xff;
	
	if( ACII >= '0' && ACII <= '9' )
	{
		result = ACII-'0';
	}
	else if( ACII >= 'a' && ACII <='f' )
	{
		result = ACII -'a' + 10;
	}
	else if( ACII >='A' && ACII <='F')
	{
		result = ACII -'A'+10;
	}
	else
	{
	}
	
	return result&0x0f;
}

uint8_t HexStr2byte(const char *instr, uint8_t *outdata, uint32_t len)
{
	volatile uint32_t i,validstrlen;
	
	if( instr == NULL || outdata == NULL || len == 0)
		return 0;
	
	/***** if it has invalid string ,return the valid string len *****/
	for(i=0;i<strlen(instr);i++)
	{
		if( instr[i] < '0' )
			break;
		
		if( instr[i] > '9' && instr[i] < 'A' )
			break;

		if( instr[i] > 'F' && instr[i] < 'a' )
			break;
		
		if( instr[i] > 'f' )
			break;
	}
	
	if(i<strlen(instr))
	{
		validstrlen = ++i;
	}
	else
	{
		validstrlen = i;
	}

	/***** if the string len is ODD, convert into EVEN. ****/
	if( validstrlen % 2 )
	{
		validstrlen = MIN(validstrlen+1,len<<1);
		
		outdata[0] = ACII2Byte(instr[0]);
		for(uint32_t i=1,j=1;i<validstrlen-2;i+=2,j++)
		{
			outdata[j] = ACII2Byte(instr[i])<<4 | ACII2Byte(instr[i+1]);
		}
	}
	else
	{
		validstrlen = MIN(validstrlen,len<<1);
		
		for(uint32_t i=0,j=0;i<validstrlen;i+=2,j++)
		{
			outdata[j] = ACII2Byte(instr[i])<<4 | ACII2Byte(instr[i+1]);
		}
	}
	
	return validstrlen>>1;
}

uint8_t HexStr2byte2(const char *instr, uint8_t *outdata, uint32_t outlen)
{
	if( instr == NULL || outdata == NULL )
		return 1;
	
	if( strlen(instr) > outlen<<1 )
		return 1;
	
	for(uint32_t i=0;i<outlen * 2;i++)
	{
		if( instr[i] < '0' )
			return 1;
		
		if( instr[i] > '9' && instr[i] < 'A' )
			return 1;

		if( instr[i] > 'F' && instr[i] < 'a' )
			return 1;
		
		if( instr[i] > 'f' )
			return 1;
	}
	
	for(uint32_t i=0,j=0;i<outlen * 2;i+=2,j++)
	{
		outdata[j] = ACII2Byte(instr[i])<<4 | ACII2Byte(instr[i+1]);
	}
	
	return 0;
}
uint8_t HexStr2Nbyte(const char *instr, uint8_t *outdata, uint32_t outlen)
{
	volatile uint32_t i,validstrlen;
	
	if( instr == NULL || outdata == NULL || outlen == 0)
		return 0;
	
	/***** if it has invalid string ,return the valid string len *****/
	for(i=0;i<strlen(instr);i++)
	{
		if( instr[i] < '0' )
			break;
		
		if( instr[i] > '9' && instr[i] < 'A' )
			break;

		if( instr[i] > 'F' && instr[i] < 'a' )
			break;
		
		if( instr[i] > 'f' )
			break;
	}
	
	validstrlen = i;
	
	/***** if the string len is ODD, convert into EVEN. ****/
	if( validstrlen % 2 )
	{
		validstrlen = MIN(validstrlen+1,outlen<<1);
		
		outdata[0] = ACII2Byte(instr[0]);
		for(uint32_t i=1,j=1;i<validstrlen-2;i+=2,j++)
		{
			//os_log("%c %c",instr[i],instr[i+1]);
			outdata[j] = ACII2Byte(instr[i])<<4 | ACII2Byte(instr[i+1]);
		}
	}
	else
	{
		validstrlen = MIN(validstrlen,outlen<<1);
		
		for(uint32_t i=0,j=0;i<validstrlen;i+=2,j++)
		{
			outdata[j] = ACII2Byte(instr[i])<<4 | ACII2Byte(instr[i+1]);
		}
	}
	
	return validstrlen>>1;
}
/**************************************************************************/

uint32_t htonl(uint32_t a) 
{
	return (uint32_t)((a>>24)&0x000000ff)|((a<<24)&0xff000000)|((a>>8)&0x0000ff00)|((a<<8)&0x00ff0000);
}

uint32_t ntohl(uint32_t a) 
{
	return (uint32_t)((a>>24)&0x000000ff)|((a<<24)&0xff000000)|((a>>8)&0x0000ff00)|((a<<8)&0x00ff0000);
}


uint32_t htonf(float f)
{  
	uint32_t * p_tmpu32 = (uint32_t *) &f;  
	
	return htonl(*p_tmpu32);
} 

float ntohf(uint32_t p)
{  
	uint32_t tmpu32 = ntohl(p);  
	float *p_tmpfloat = (float *)&tmpu32;  
	
	return (*p_tmpfloat);
}
