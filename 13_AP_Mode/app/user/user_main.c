/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														//													//
// ���̣�	AP_Mode										// ��֤��	�٣��ҵ���Ϊ��ESP8266_JX����WIFI		//
//														//													//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//			�ڣ��������롰jixiaoxin��������WIFI		//
//														//													//
// ���ܣ�	�٣�ESP8266����ΪAPģʽ������WIFI������		//			�ۣ��鿴����/�ֻ���IP��ַ				//
//														//													//
//			�ڣ���ʱ��ӡESP8266_softAPģʽ�µ�IP��ַ	//			�ܣ�STA��IP��ַӦΪ��192.168.4.X����	//
//														//													//
//			�ۣ���ʱ��ѯ����ӡ���Ӵ�WIFI���豸����		//////////////////////////////////////////////////////
//														//													//
//			�ܣ����ֻ�/���Խ����WIFI					//		AP(ESP8266)	= 192.168.4.1	Ĭ�Ͽ���DHCP	//
//														//													//
//			�ݣ��鿴���Ե�IP��ַ��192,.168.4.1			// 		STA(������)	= 192.168.4.2	Ĭ�Ͽ���DHCP	//
//														//													//
// �汾��	V1.0										//		STA(������)	= 192.168.4.3	Ĭ�Ͽ���DHCP	//
//														//													//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED

//#include "at_custom.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
//#include "espconn.h"
//#include "espnow.h"
#include "ets_sys.h"			// �ص�����
//#include "gpio.h"
//#include "ip_addr.h"
//#include "mem.h"
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX�������ʱ��
//#include "ping.h"
//#include "pwm.h"
//#include "queue.h"
//#include "smartconfig.h"
//#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro
//==================================================================================


// �궨��
//==================================================================================
#define		ProjectName			"AP_Mode"			// �������궨��

#define		ESP8266_AP_SSID		"ESP8266_JX"		// ������WIFI��
#define		ESP8266_AP_PASS		"jixiaoxin"			// ������WIFI����
//==================================================================================


// ȫ�ֱ���
//==================================================================================
os_timer_t OS_Timer_1;		// �����ʱ��
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ��ʼ��ESP8266_APģʽ
//============================================================================================
void ICACHE_FLASH_ATTR ESP8266_AP_Init_JX()
{
	struct softap_config AP_Config;				// AP�����ṹ��

	wifi_set_opmode(0x02);						// ����ΪAPģʽ�������浽Flash

	// �ṹ�帳ֵ(ע�⣺�����񼯱�ʶ��/���롿����Ϊ�ַ�����ʽ)
	//--------------------------------------------------------------------------------------
	os_memset(&AP_Config, 0, sizeof(struct softap_config));	// AP�����ṹ�� = 0
	os_strcpy(AP_Config.ssid,ESP8266_AP_SSID);		// ����SSID(���ַ������Ƶ�ssid����)
	os_strcpy(AP_Config.password,ESP8266_AP_PASS);	// ��������(���ַ������Ƶ�password����)
	AP_Config.ssid_len=os_strlen(ESP8266_AP_SSID);	// ����ssid����(��SSID�ĳ���һ��)
	AP_Config.channel=1;                      		// ͨ����1��13
	AP_Config.authmode=AUTH_WPA2_PSK;           	// ���ü���ģʽ
	AP_Config.ssid_hidden=0;                  		// ������SSID
	AP_Config.max_connection=4;               		// ���������
	AP_Config.beacon_interval=100;            		// �ű���ʱ��100��60000 ms

	wifi_softap_set_config(&AP_Config);				// ����soft-AP�������浽Flash
}
//============================================================================================


// ��ʱ�Ļص�����
//=====================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	struct ip_info ST_ESP8266_IP;	// IP��Ϣ�ṹ��

	u8  ESP8266_IP[4];		// ���ʮ������ʽ����IP


	// ��ѯ����ӡESP8266�Ĺ���ģʽ
	//---------------------------------------------------------------------
	switch(wifi_get_opmode())	// �������ģʽ
	{
		case 0x01:	os_printf("\nESP8266_Mode = Station\n");		break;
		case 0x02:	os_printf("\nESP8266_Mode = SoftAP\n");			break;
		case 0x03:	os_printf("\nESP8266_Mode = Station+SoftAP\n");	break;
	}


	// ��ȡESP8266_APģʽ�µ�IP��ַ
	//��APģʽ�£��������DHCP(Ĭ��)������δ����IP��ز�����ESP8266��IP��ַ=192.168.4.1��
	//-----------------------------------------------------------------------------------
	wifi_get_ip_info(SOFTAP_IF,&ST_ESP8266_IP);	// ����2��IP��Ϣ�ṹ��ָ��

	// ESP8266_AP_IP.ip.addr==32λ������IP��ַ������ת��Ϊ���ʮ���Ƶ���ʽ
	//------------------------------------------------------------------------------------------
	ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;			// ���ʮ����IP�ĵ�һ���� <==> addr�Ͱ�λ
	ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;		// ���ʮ����IP�ĵڶ����� <==> addr�εͰ�λ
	ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;		// ���ʮ����IP�ĵ������� <==> addr�θ߰�λ
	ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;		// ���ʮ����IP�ĵ��ĸ��� <==> addr�߰�λ

	// ��ӡESP8266��IP��ַ
	//-----------------------------------------------------------------------------------------------
	os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
	OLED_ShowIP(24,2,ESP8266_IP);			// ��ʾESP8266��IP��ַ


	// ��ѯ����ӡ�����WIFI���豸����
	//-----------------------------------------------------------------------------------------
	os_printf("Number of devices connected to this WIFI = %d\n",wifi_softap_get_station_num());
}
//=====================================================================================================


// �����ʱ����ʼ��(ms����)
//=====================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//=====================================================================================


// user_init��entry of user application, init user function here
//==================================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// OLED��ʼ��
	//������������������������������������������
	OLED_Init();							// |
	OLED_ShowString(0,0,"ESP8266 = AP");	// |
	OLED_ShowString(0,2,"IP:");				// |
	//������������������������������������������


	ESP8266_AP_Init_JX();			// ����ESP8266_APģʽ��ز���

    OS_Timer_1_Init_JX(1000,1);		// 1�������ʱ


//	while(1) system_soft_wdt_feed();	// ��ѭ����������

    os_printf("\r\n-------------------- user_init OVER --------------------\r\n");
}
//==================================================================================


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void){}
