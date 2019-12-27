/* * ESPRESSIF MIT License
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

//////////////////////////////////////////////////////////
//														//
// ���̣�	STA_Mode									//
//														//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//
//														//
// ���ܣ�	�٣�8266����ΪSTAģʽ������WIFI�ȵ�			//
//														//
//			�ڣ�ÿ���ѯESP8266����WIFI��״̬			//
//														//
//			�ۣ����ڴ�ӡ����״̬/IP��ַ					//
//														//
//	�汾��	V1.0										//
//														//
//////////////////////////////////////////////////////////



// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED

//#include "at_custom.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ip_addr.h"			// ��"espconn.h"ʹ�á���"espconn.h"��ͷ#include"ip_addr.h"��#include"ip_addr.h"����"espconn.h"֮ǰ
#include "espconn.h"			// TCP/UDP�ӿ�
//#include "espnow.h"
#include "ets_sys.h"			// �ص�����
//#include "gpio.h"
#include "mem.h"				// �ڴ�����Ⱥ���
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
#define		ProjectName			"STA_Mode"				// �������궨��

#define		ESP8266_STA_SSID	"JI-XIN-TEC"			// �����WIFI��
#define		ESP8266_STA_PASS	"JIXINDIANZI78963"		// �����WIFI����

#define		LED_ON				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0)		// LED��
#define		LED_OFF				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1)		// LED��
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


// ESP8266_STA��ʼ��
//==============================================================================
void ICACHE_FLASH_ATTR ESP8266_STA_Init_JX()
{
	struct station_config STA_Config;	// STA�����ṹ��

	struct ip_info ST_ESP8266_IP;		// STA��Ϣ�ṹ��

	// ����ESP8266�Ĺ���ģʽ
	//------------------------------------------------------------------------
	wifi_set_opmode(0x01);				// ����ΪSTAģʽ�������浽Flash

	/*
	// ����STAģʽ�µ�IP��ַ��ESP8266Ĭ�Ͽ���DHCP Client������WIFIʱ���Զ�����IP��ַ��
	//--------------------------------------------------------------------------------
	wifi_station_dhcpc_stop();						// �ر� DHCP Client
	IP4_ADDR(&ST_ESP8266_IP.ip,192,168,8,88);		// ����IP��ַ
	IP4_ADDR(&ST_ESP8266_IP.netmask,255,255,255,0);	// ������������
	IP4_ADDR(&ST_ESP8266_IP.gw,192,168,8,1);		// �������ص�ַ
	wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����STAģʽ�µ�IP��ַ
	*/

	// �ṹ�帳ֵ������STAģʽ����
	//-------------------------------------------------------------------------------
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA�����ṹ�� = 0
	os_strcpy(STA_Config.ssid,ESP8266_STA_SSID);				// ����WIFI��
	os_strcpy(STA_Config.password,ESP8266_STA_PASS);			// ����WIFI����

	wifi_station_set_config(&STA_Config);	// ����STA�����������浽Flash


	// ��API������user_init��ʼ���е���
	// ���user_init�е���wifi_station_set_config(...)�Ļ����ں˻��Զ���ESP8266����WIFI
	//----------------------------------------------------------------------------------
	// wifi_station_connect();		// ESP8266����WIFI
}
//=========================================================================================


// �����ʱ�Ļص�����
//========================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	u8 C_LED_Flash = 0;				// LED��˸�ƴ�

	u8 S_WIFI_STA_Connect;			// WIFI����״̬��־

	struct ip_info ST_ESP8266_IP;	// ESP8266��IP��Ϣ
	u8 ESP8266_IP[4];				// ESP8266��IP��ַ

	// ��ѯSTA����WIFI״̬
	//-----------------------------------------------------
	S_WIFI_STA_Connect = wifi_station_get_connect_status();
	//---------------------------------------------------
	// Station����״̬��
	// 0 == STATION_IDLE -------------- STATION����
	// 1 == STATION_CONNECTING -------- ��������WIFI
	// 2 == STATION_WRONG_PASSWORD ---- WIFI�������
	// 3 == STATION_NO_AP_FOUND ------- δ����ָ��WIFI
	// 4 == STATION_CONNECT_FAIL ------ ����ʧ��
	// 5 == STATION_GOT_IP ------------ ���IP�����ӳɹ�
	//---------------------------------------------------

	switch(S_WIFI_STA_Connect)
	{
		case 0 : 	os_printf("\nSTATION_IDLE\n");				break;
		case 1 : 	os_printf("\nSTATION_CONNECTING\n");		break;
		case 2 : 	os_printf("\nSTATION_WRONG_PASSWORD\n");	break;
		case 3 : 	os_printf("\nSTATION_NO_AP_FOUND\n");		break;
		case 4 : 	os_printf("\nSTATION_CONNECT_FAIL\n");		break;
		case 5 : 	os_printf("\nSTATION_GOT_IP\n");			break;
	}

	// �ɹ�����WIFI��STAģʽ�£��������DHCP(Ĭ��)����ESO8266��IP��ַ��WIFI·�����Զ����䡿
	//----------------------------------------------------------------------------------------
	if( S_WIFI_STA_Connect == STATION_GOT_IP )	// �ж��Ƿ��ȡIP
	{
		// ��ȡESP8266_Stationģʽ�µ�IP��ַ
		// DHCP-ClientĬ�Ͽ�����ESP8266����WIFI����·��������IP��ַ��IP��ַ��ȷ��
		//--------------------------------------------------------------------------
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����2��IP��Ϣ�ṹ��ָ��

		// ESP8266_AP_IP.ip.addr��32λ�����ƴ��룬ת��Ϊ���ʮ������ʽ
		//----------------------------------------------------------------------------
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// IP��ַ�߰�λ == addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// IP��ַ�θ߰�λ == addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// IP��ַ�εͰ�λ == addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// IP��ַ�Ͱ�λ == addr�߰�λ

		// ��ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(24,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------

		// ����WIFI�ɹ���LED����3��
		//----------------------------------------------------
		for(; C_LED_Flash<=5; C_LED_Flash++)
		{
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),(C_LED_Flash%2));
			delay_ms(100);
		}

		os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	}
}
//========================================================================================================

// �����ʱ����ʼ��(ms����)
//=====================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//=====================================================================================

// LED��ʼ��
//=============================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}
//=============================================================================

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
	OLED_ShowString(0,0,"ESP8266 = STA");	// |
	OLED_ShowString(0,2,"IP:");				// |
	//������������������������������������������

	LED_Init_JX();		// LED��ʼ��


	ESP8266_STA_Init_JX();			// ESP8266_STA��ʼ��

	OS_Timer_1_Init_JX(1000,1);		// ��ʱ��ѯWIFI����״̬
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
