#include <ctype.h>
#include "AepServiceCodes.h"
#include "com.h"
//无符号整型16位  
uint_16 aep_htons(uint_16 source)  
{  

	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
		return (uint_16)( 0
		| ((source & 0x00ff) << 8)
		| ((source & 0xff00) >> 8) );  
}  

//无符号整型32位
uint_32 aep_htoni(uint_32 source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
		return 0
		| ((source & 0x000000ff) << 24)
		| ((source & 0x0000ff00) << 8)
		| ((source & 0x00ff0000) >> 8)
		| ((source & 0xff000000) >> 24);  
}

//无符号整型64位
uint_64 aep_htonl(uint_64 source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
		return 0
		| ((source & (uint_64)(0x00000000000000ff)) << 56)
		| ((source & (uint_64)(0x000000000000ff00)) << 40)
		| ((source & (uint_64)(0x0000000000ff0000)) << 24)
		| ((source & (uint_64)(0x00000000ff000000)) << 8)
		| ((source & (uint_64)(0x000000ff00000000)) >> 8)
		| ((source & (uint_64)(0x0000ff0000000000)) >> 24)
		| ((source & (uint_64)(0x00ff000000000000)) >> 40)
		| ((source & (uint_64)(0xff00000000000000)) >> 56);
}

//float
float aep_htonf(float source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
	{
		uint_32 t= 0
			| ((*(uint_32*)&source & 0x000000ff) << 24)
			| ((*(uint_32*)&source & 0x0000ff00) << 8)
			| ((*(uint_32*)&source & 0x00ff0000) >> 8)
			| ((*(uint_32*)&source & 0xff000000) >> 24);
		return *(float*)&t;
	} 
}

//double
double aep_htond(double source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
	{
		uint_64 t= 0
			| ((*(uint_64*)&source & (uint_64)(0x00000000000000ff)) << 56)
			| ((*(uint_64*)&source & (uint_64)(0x000000000000ff00)) << 40)
			| ((*(uint_64*)&source & (uint_64)(0x0000000000ff0000)) << 24)
			| ((*(uint_64*)&source & (uint_64)(0x00000000ff000000)) << 8)
			| ((*(uint_64*)&source & (uint_64)(0x000000ff00000000)) >> 8)
			| ((*(uint_64*)&source & (uint_64)(0x0000ff0000000000)) >> 24)
			| ((*(uint_64*)&source & (uint_64)(0x00ff000000000000)) >> 40)
			| ((*(uint_64*)&source & (uint_64)(0xff00000000000000)) >> 56);
		return *(double*)&t;
	}
}

//16进制转字符串
void HexToStr(char *pbDest, char *pbSrc, int nLen)
{
	unsigned char ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + (unsigned char)pbSrc[i] / 16;
		ddl = 48 + (unsigned char)pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	//pbDest[nLen*2] = '\0';
}

//字符串转16进制
void StrToHex(char *pbDest, char *pbSrc, int nLen)
{
	unsigned char h1,h2;
	unsigned char s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
			s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

//数据上报:业务数据上报
AepString data_report_CodeDataReport (data_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 4;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.power_consumption = aep_htonf(srcStruct.power_consumption);

	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(1);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.power_consumption, 4);
	index += 4 * 2;


	return resultStruct;
}

//指令下发:获取用电量
int get_power_consumption_DecodeCmdDown (char* source, get_power_consumption* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 4;


	memset(dest, 0, sizeof(get_power_consumption));

	StrToHex((char *)&dest->cmd_type, index, 4);
	dest->cmd_type = aep_htoni(dest->cmd_type);
	index += 4 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:获取用电量响应
AepString get_power_consumption_resp_CodeCmdResponse (get_power_consumption_resp srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 4;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.power_consumption = aep_htonf(srcStruct.power_consumption);

	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9001);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.power_consumption, 4);
	index += 4 * 2;


	return resultStruct;
}

//指令下发:电机速度控制
int motor_speed_control_DecodeCmdDown (char* source, motor_speed_control* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 2;


	memset(dest, 0, sizeof(motor_speed_control));

	StrToHex((char *)&dest->motor_speed_control, index, 2);
	dest->motor_speed_control = aep_htons(dest->motor_speed_control);
	index += 2 * 2;
	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	SetMotorSpeed(dest->motor_speed_control);
	return AEP_CMD_SUCCESS;
}

//指令下发:拉闸合闸
int set_onoff_DecodeCmdDown (char* source, set_onoff* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 1;


	memset(dest, 0, sizeof(set_onoff));

	StrToHex((char *)&dest->onoff, index, 1);
	index += 1 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:拉闸合闸响应
AepString set_onoff_resp_CodeCmdResponse (set_onoff_resp srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 1;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);


	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9002);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.onoff, 1);
	index += 1 * 2;


	return resultStruct;
}

//指令下发:设置心跳时间
int set_reporttime_DecodeCmdDown (char* source, set_reporttime* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 2;


	memset(dest, 0, sizeof(set_reporttime));

	StrToHex((char *)&dest->Report_Time, index, 2);
	dest->Report_Time = aep_htons(dest->Report_Time);
	index += 2 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:设置心跳时间相应
AepString set_reporttime_ack_CodeCmdResponse (set_reporttime_ack srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 2;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.Report_Time = aep_htons(srcStruct.Report_Time);

	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9003);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.Report_Time, 2);
	index += 2 * 2;


	return resultStruct;
}

//数据上报:信号数据上报
AepString signal_report_CodeDataReport (signal_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 20;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.rsrp = aep_htoni(srcStruct.rsrp);
	srcStruct.sinr = aep_htoni(srcStruct.sinr);
	srcStruct.pci = aep_htoni(srcStruct.pci);
	srcStruct.ecl = aep_htoni(srcStruct.ecl);
	srcStruct.cell_id = aep_htoni(srcStruct.cell_id);

	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(2);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.rsrp, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.sinr, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.pci, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.ecl, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.cell_id, 4);
	index += 4 * 2;


	return resultStruct;
}

//事件上报:拉合闸状态上报
AepString state_alarm_CodeEventReport (state_alarm srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 1;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);


	index = resultStruct.str;

	memcpy(index, "07", 2);
	index += 1 * 2;

	tempLen = aep_htons(1001);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.onoff, 1);
	index += 1 * 2;


	return resultStruct;
}

//数据上报:电表状态信息上报
AepString status_report_CodeDataReport (status_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 84;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.A_Volt = aep_htonf(srcStruct.A_Volt);
	srcStruct.B_Volt = aep_htonf(srcStruct.B_Volt);
	srcStruct.C_Volt = aep_htonf(srcStruct.C_Volt);
	srcStruct.A_Ele = aep_htonf(srcStruct.A_Ele);
	srcStruct.B_Ele = aep_htonf(srcStruct.B_Ele);
	srcStruct.C_Ele = aep_htonf(srcStruct.C_Ele);
	srcStruct.A_Waste = aep_htonf(srcStruct.A_Waste);
	srcStruct.B_Waste = aep_htonf(srcStruct.B_Waste);
	srcStruct.C_Waste = aep_htonf(srcStruct.C_Waste);
	srcStruct.A_WasteFactor = aep_htonf(srcStruct.A_WasteFactor);
	srcStruct.B_WasteFactor = aep_htonf(srcStruct.B_WasteFactor);
	srcStruct.C_WasteFactor = aep_htonf(srcStruct.C_WasteFactor);
	srcStruct.Report_Time = aep_htons(srcStruct.Report_Time);
	srcStruct.Message_Cnt = aep_htoni(srcStruct.Message_Cnt);

	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(3);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)srcStruct.devIMEI, 15);
	index += 15 * 2;

	HexToStr(index, (char *)srcStruct.devIMSI, 15);
	index += 15 * 2;

	HexToStr(index, (char *)&srcStruct.A_Volt, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.B_Volt, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.C_Volt, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.A_Ele, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.B_Ele, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.C_Ele, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.A_Waste, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.B_Waste, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.C_Waste, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.A_WasteFactor, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.B_WasteFactor, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.C_WasteFactor, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.Report_Time, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.Message_Cnt, 4);
	index += 4 * 2;


	return resultStruct;
}

AepCmdData decodeCmdDownFromStr(char* source)
{
	char* index;
	AepCmdData result;
	char cmdType;
	unsigned short serviceId;
	unsigned short payloadLen;

	memset(&result, 0, sizeof(AepCmdData));

	index = source;

	//解析指令类型
	StrToHex(&cmdType, index, 1);
	index += 1 * 2;
	if (cmdType != 0x06)
	{
		result.code = AEP_CMD_INVALID_DATASET_TYPE;
	}

	//服务Id解析
	StrToHex((char *)&serviceId, index, 2);
	serviceId = aep_htons(serviceId);
	index += 2 * 2;

	StrToHex((char *)&result.taskId, index, 2);
	result.taskId = aep_htons(result.taskId);
	index += 2 * 2;

	//payload长度解析
	StrToHex((char *)&payloadLen, index, 2);
	payloadLen = aep_htons(payloadLen);
	index += 2 * 2;

	if (strlen(index) < payloadLen * 2)
	{
		result.code = AEP_CMD_PAYLOAD_PARSING_FAILED;
		return result;
	}


	if (serviceId == 8001)
	{
		result.serviceIdentifier = "get_power_consumption";
		result.data = malloc(sizeof(get_power_consumption));
		memset(result.data, 0, sizeof(get_power_consumption));
		result.code = get_power_consumption_DecodeCmdDown(index, (get_power_consumption*)result.data);
	}
	else if (serviceId == 8004)
	{
		result.serviceIdentifier = "motor_speed_control";
		result.data = malloc(sizeof(motor_speed_control));
		memset(result.data, 0, sizeof(motor_speed_control));
		result.code = motor_speed_control_DecodeCmdDown(index, (motor_speed_control*)result.data);
	}
	else if (serviceId == 8002)
	{
		result.serviceIdentifier = "set_onoff";
		result.data = malloc(sizeof(set_onoff));
		memset(result.data, 0, sizeof(set_onoff));
		result.code = set_onoff_DecodeCmdDown(index, (set_onoff*)result.data);
	}
	else if (serviceId == 8003)
	{
		result.serviceIdentifier = "set_reporttime";
		result.data = malloc(sizeof(set_reporttime));
		memset(result.data, 0, sizeof(set_reporttime));
		result.code = set_reporttime_DecodeCmdDown(index, (set_reporttime*)result.data);
	}
	else 
	{
		result.serviceIdentifier = NULL;
		result.data = malloc(payloadLen);
		memset(result.data, 0, sizeof(payloadLen));
		StrToHex((char *)result.data, index, payloadLen);
		result.code = AEP_CMD_INVALID_DATASET_IDENTIFIER;
	}

	return result;
}

AepCmdData decodeCmdDownFromBytes(char* source, int len)
{
	char * str = malloc(len * 2 + 1);
	AepCmdData result;
	HexToStr(str, source, len);
	str[len * 2] = 0;
	
	result = decodeCmdDownFromStr(str);
	free(str);
	return result;
}

AepString codeDataReportByIdToStr (int serviceId, void * srcStruct)
{
	if (serviceId == 1)
	{
		return data_report_CodeDataReport(*(data_report*)srcStruct);
	}
	else if (serviceId == 9001)
	{
		return get_power_consumption_resp_CodeCmdResponse(*(get_power_consumption_resp*)srcStruct);
	}
	else if (serviceId == 9002)
	{
		return set_onoff_resp_CodeCmdResponse(*(set_onoff_resp*)srcStruct);
	}
	else if (serviceId == 9003)
	{
		return set_reporttime_ack_CodeCmdResponse(*(set_reporttime_ack*)srcStruct);
	}
	else if (serviceId == 2)
	{
		return signal_report_CodeDataReport(*(signal_report*)srcStruct);
	}
	else if (serviceId == 1001)
	{
		return state_alarm_CodeEventReport(*(state_alarm*)srcStruct);
	}
	else if (serviceId == 3)
	{
		return status_report_CodeDataReport(*(status_report*)srcStruct);
	}
	else 
	{
		AepString result = {0};
		return result;
	}
}

AepBytes codeDataReportByIdToBytes(int serviceId, void * srcStruct)
{
	AepString temp = codeDataReportByIdToStr(serviceId, srcStruct);
	AepBytes result = {0};
	result.len = temp.len / 2;
	if (result.len > 0)
	{
		result.str = malloc(result.len);
		StrToHex(result.str, temp.str, result.len);
		free(temp.str);
	}
	return result;
}

AepString codeDataReportByIdentifierToStr (char* serviceIdentifier, void * srcStruct)
{
	if (strcmp(serviceIdentifier, "data_report") == 0)
	{
		return data_report_CodeDataReport(*(data_report*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "get_power_consumption_resp") == 0)
	{
		return get_power_consumption_resp_CodeCmdResponse(*(get_power_consumption_resp*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "set_onoff_resp") == 0)
	{
		return set_onoff_resp_CodeCmdResponse(*(set_onoff_resp*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "set_reporttime_ack") == 0)
	{
		return set_reporttime_ack_CodeCmdResponse(*(set_reporttime_ack*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "signal_report") == 0)
	{
		return signal_report_CodeDataReport(*(signal_report*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "state_alarm") == 0)
	{
		return state_alarm_CodeEventReport(*(state_alarm*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "status_report") == 0)
	{
		return status_report_CodeDataReport(*(status_report*)srcStruct);
	}
	else 
	{
		AepString result = {0};
		return result;
	}
}

AepBytes codeDataReportByIdentifierToBytes(char* serviceIdentifier, void * srcStruct)
{
	AepString temp = codeDataReportByIdentifierToStr(serviceIdentifier, srcStruct);
	AepBytes result = {0};
	result.len = temp.len / 2;
	if (result.len > 0)
	{
		result.str = malloc(result.len);
		StrToHex(result.str, temp.str, result.len);
		free(temp.str);
	}
	return result;
}

