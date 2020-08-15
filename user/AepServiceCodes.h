/***
*AepServiceCodes.h - 定义上下行数据的结构体，还有提供组装上行报文的函数和解析下行报文的函数
*
*Purpose:
*	1.数据结构体命名和平台定义的服务标识一致
*	2.codeDataReportByIdToStr、codeDataReportByIdToBytes、codeDataReportByIdentifierToStr、codeDataReportByIdentifierToBytes为组装上报数据的函数，具体说明见函数前的注释
*	3.decodeCmdDownFromStr、decodeCmdDownFromBytes为解析平台发送过来数据的函数，具体说明见函数前的注释
****/
#ifndef AEPSERVICECODES_H
#define AEPSERVICECODES_H

#include <stdlib.h>
#include <string.h>


#define AEP_BIG_ENDIAN 'b'
#define AEP_LITTLE_ENDIAN 'l'

static union { char c[4]; unsigned long mylong; } endian_test = {{ 'l', '?', '?', 'b' } };
#define AEP_ENDIANNESS ((char)endian_test.mylong)


typedef unsigned long long uint_64;
typedef unsigned int uint_32;  
typedef unsigned short uint_16;

//命令解析响应码
#define AEP_CMD_SUCCESS 0						//执行成功
#define AEP_CMD_FAILED 1						//执行失败
#define AEP_CMD_INVALID_DATASET_TYPE 2			//无效数据集类型
#define AEP_CMD_INVALID_DATASET_IDENTIFIER 3	//无效数据集标识
#define AEP_CMD_PAYLOAD_PARSING_FAILED 4		//指令数据集Payload解析失败,紧凑二进制编码内容长度不符等


typedef struct AepStrStruct
{
	unsigned short len;
	char* str;
} AepString;
typedef AepString AepBytes;

//无符号整型16位  
uint_16 aep_htons(uint_16 source);

//无符号整型32位
uint_32 aep_htoni(uint_32 source);

//无符号整型64位
uint_64 aep_htonl(uint_64 source);

//float
float aep_htonf(float source);

//double
double aep_htond(double source);

//16进制转字符串
void HexToStr(char *pbDest, char *pbSrc, int nLen);

//字符串转16进制
void StrToHex(char *pbDest, char *pbSrc, int nLen);


//根据服务id生成上报数据的十六进制字符串,srcStruct需要根据服务定义传入对应的类型,返回结果为字符串
AepString codeDataReportByIdToStr(int serviceId, void * srcStruct);

//根据服务id生成上报数据的字节流,srcStruct需要根据服务定义传入对应的类型,返回结果为字节流
AepBytes codeDataReportByIdToBytes(int serviceId, void * srcStruct);

//根据服务标识生成上报数据的十六进制字符串,srcStruct需要根据服务定义传入对应的类型,返回结果为字符串
AepString codeDataReportByIdentifierToStr(char * serviceIdentifier, void * srcStruct);

//根据服务标识生成上报数据的字节流,srcStruct需要根据服务定义传入对应的类型,返回结果为字节流
AepBytes codeDataReportByIdentifierToBytes(char * serviceIdentifier, void * srcStruct);

//指令解析返回结构体，data在使用时需要根据serviceId强转为对应类型
typedef struct CmdStruct 
{
	char* serviceIdentifier;
	unsigned short taskId;
	void * data;
	int code;
} AepCmdData;
//解析接受到的报文数据,入参为十六进制字符串
AepCmdData decodeCmdDownFromStr(char* source);
//解析接受到的报文数据,入参为原始字节流
AepCmdData decodeCmdDownFromBytes(char* source, int len);



typedef struct data_reportStruct 
{
	float power_consumption;
} data_report;
//数据上报:业务数据上报
AepString data_report_CodeDataReport (data_report srcStruct);


typedef struct get_power_consumptionStruct 
{
	int cmd_type;
} get_power_consumption;
//指令下发:获取用电量
int get_power_consumption_DecodeCmdDown (char* source, get_power_consumption* dest);


typedef struct get_power_consumption_respStruct 
{
	unsigned short taskId;
	float power_consumption;
} get_power_consumption_resp;
//指令下发响应:获取用电量响应
AepString get_power_consumption_resp_CodeCmdResponse (get_power_consumption_resp srcStruct);


typedef struct motor_speed_controlStruct 
{
	short motor_speed_control;
} motor_speed_control;
//指令下发:电机速度控制
int motor_speed_control_DecodeCmdDown (char* source, motor_speed_control* dest);


typedef struct set_onoffStruct 
{
	char onoff;
} set_onoff;
//指令下发:拉闸合闸
int set_onoff_DecodeCmdDown (char* source, set_onoff* dest);


typedef struct set_onoff_respStruct 
{
	unsigned short taskId;
	char onoff;
} set_onoff_resp;
//指令下发响应:拉闸合闸响应
AepString set_onoff_resp_CodeCmdResponse (set_onoff_resp srcStruct);


typedef struct set_reporttimeStruct 
{
	short Report_Time;
} set_reporttime;
//指令下发:设置心跳时间
int set_reporttime_DecodeCmdDown (char* source, set_reporttime* dest);


typedef struct set_reporttime_ackStruct 
{
	unsigned short taskId;
	short Report_Time;
} set_reporttime_ack;
//指令下发响应:设置心跳时间相应
AepString set_reporttime_ack_CodeCmdResponse (set_reporttime_ack srcStruct);


typedef struct signal_reportStruct 
{
	int rsrp;
	int sinr;
	int pci;
	int ecl;
	int cell_id;
} signal_report;
//数据上报:信号数据上报
AepString signal_report_CodeDataReport (signal_report srcStruct);


typedef struct state_alarmStruct 
{
	char onoff;
} state_alarm;
//事件上报:拉合闸状态上报
AepString state_alarm_CodeEventReport (state_alarm srcStruct);


typedef struct status_reportStruct 
{
	char devIMEI[15];
	char devIMSI[15];
	float A_Volt;
	float B_Volt;
	float C_Volt;
	float A_Ele;
	float B_Ele;
	float C_Ele;
	float A_Waste;
	float B_Waste;
	float C_Waste;
	float A_WasteFactor;
	float B_WasteFactor;
	float C_WasteFactor;
	short Report_Time;
	int Message_Cnt;
} status_report;
//数据上报:电表状态信息上报
AepString status_report_CodeDataReport (status_report srcStruct);



#endif
