#ifndef  __COM_H__
#define  __COM_H__

#include "main.h"
#include <time.h>

#define AT_RESPONSE_OK   0
#define AT_RESPONSE_ERROR 1
#define AT_RESPONSE_NULL  2





typedef enum
{
    NET_DOWN,
    NET_UP
} NET_STATUS;

/*
AT+NUESTATS
Signal power:-842
Total power:-780
TX power:100
TX time:859
RX time:26543
Cell ID:137262770   小区位置信息
ECL:0               无线信号覆盖等级
SNR:226               信噪比
EARFCN:3734
PCI:105             小区物理标示
RSRQ:-108     参考信号接收质量。实际值需要除以十
OPERATOR MODE:4
CURRENT BAND:8
OK
*/
typedef __packed struct module_stats
{
    int signal_power;
    int total_power;
    uint16_t tx_power;
    uint16_t tx_time;
    uint16_t rx_time;
    uint8_t current_band;
    int cell_id;
    int pci;
    int prsq;
    int snr;
    int ecl;
} module_stats;

/*
AT+CIMI
460113037624468   //imsi号
OK

AT+NCCID   //sim卡号
+NCCID:89861119215005126033
OK

AT+CGPADDR   //本机IP
+CGPADDR:0,11.81.58.63
OK



AT+CGSN=1 //请求 IMEI 号
+CGSN:490154203237511



AT+CGSN       //序列号
MPD19KD05018982
OK

*/


typedef __packed struct module_info
{
    uint8_t imei[15];
    uint8_t imsi[15];
    uint8_t sn[15];
    uint8_t ccid[20];
    uint8_t local_ip[15];
} module_info;





uint8_t key_scan(void);

uint8_t  at_send_cmd(uint8_t *cmd, uint8_t *resp, uint16_t readlen, uint16_t waittime);

uint8_t crc_sum_check(uint8_t *buff, uint16_t len);

uint8_t rt_rtc_set_time(time_t timestamp, uint32_t Format);

time_t get_module_time(uint8_t time_zone);

uint8_t  calibration_rtc_from_iot_module(void);


void reboot_module(void);

void SetMotorSpeed(int speed);


#endif
