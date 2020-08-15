#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdbool.h>
#include <stdint.h>

#define  LITTLE_ENDIAN      0
#define  BIG_ENDIAN         1

#define MIN(a,b)   (a)>(b)?(b):(a)


typedef union
{
    struct
    {
        unsigned char low_byte;
        unsigned char mlow_byte;
        unsigned char mhigh_byte;
        unsigned char high_byte;
    }float_byte;
    float value;
}FLOAT_UNION;

typedef union
{
    struct
    {
        unsigned char low_byte0;
        unsigned char low_byte1;
        unsigned char low_byte2;
        unsigned char low_byte3;
        unsigned char high_byte0;
        unsigned char high_byte1;
        unsigned char high_byte2;
        unsigned char high_byte3;
    }double_byte;
    double value;
}DOUBLE_UNION;

typedef union {
    int number;
    char s;
}BYTE_HIGH_LOW;


#define SPLIT_PARAMS_MAX  20
typedef struct
{
	uint8_t argc;
	char *args[SPLIT_PARAMS_MAX];
}args_block_t;

bool isLittleEndian(void);

void htonf_hexstr1(float data,char *dst);
void htonf_hexstr2(float data,char *dst);
void htonf_hexstr3(float data,char *dst);

void generate_double_data(double data,char *dst);

char m_strtok(char *in, char **out, uint8_t *argc);
char t_strtok(char *in, char *targe, char **args, uint8_t *argc);
char t_strtok2(char *in, char *targe, char **args, uint8_t *argc);
int8_t n_strtok(char *in, char *targe, char **args, uint8_t *argc);
int8_t shell_split(char *in, char *targe, char **args, uint8_t *argc);

uint32_t bytes_to_hex(const char *pSrc, char *pDst, uint32_t nSrcLength);
uint8_t HexStr2byte(const char *instr, uint8_t *outdata, uint32_t len);
uint8_t HexStr2byte2(const char *instr, uint8_t *outdata, uint32_t len);

uint8_t HexStr2Nbyte(const char *instr, uint8_t *outdata, uint32_t len);

#endif
