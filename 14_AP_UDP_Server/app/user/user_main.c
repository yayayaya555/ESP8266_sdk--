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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														//																	//
// ���̣�	AP_UDP_Server								// 	�٣�����/�ֻ�����ESP8266������WIFI								//
//														//																	//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//	�ڣ�������������������UDPͨ��(�����ѡ������ΪUDP_Client)	//
//														//																	//
// ���ܣ�	�٣�8266����ΪAPģʽ������WIFI������		//	�ۣ����õ���/�ֻ��˵�IP/�˿ڣ�IP=192.168.4.X���˿�=8888��		//
//														//																	//
//			�ڣ�����UDPͨ�ţ��߼���ESP8266��ΪServer	//	�ܣ�����Ŀ��(Server)��IP�Ͷ˿ڣ�IP=192.168.4.1 �˿�=8266��		//
//														//																	//
//			�ۣ��ȴ�Client(����/�ֻ�)�������ݹ���		//	�ݣ�����/����UDPͨ��											//
//														//																	//
//			�ܣ����ݽ��յ������ݿ���LED����/��			//	�ޣ���Server(ESP8266)�������ݡ�									//
//														//																	//
//	�汾��	V1.0										//	�ߣ�������ĸΪ'k'/'K'����������������ĸΪ'g'/'G'������		//
//														//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
#define		ProjectName			"AP_UDP_Server"		// �������궨��

#define		ESP8266_AP_SSID		"ESP8266_JX"		// ������WIFI��
#define		ESP8266_AP_PASS		"jixiaoxin"			// ������WIFI����

#define		LED_ON				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0)		// LED��
#define		LED_OFF				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1)		// LED��
//==================================================================================

// ȫ�ֱ���
//==================================================================================
os_timer_t OS_Timer_1;		// �����ʱ��

//struct espconn ST_NetCon;	// ע�����붨��Ϊȫ�ֱ������ں˽���ʹ�ô˱���(�ڴ�)
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


// �ɹ������������ݵĻص�����
//==========================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb_JX(void *arg)
{
	os_printf("\nESP8266_WIFI_Send_OK\n");
}
//==========================================================


// �ɹ������������ݵĻص�����������1�����紫��ṹ��espconnָ�롢����2�����紫������ָ�롢����3�����ݳ��ȡ�
//=========================================================================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Recv_Cb_JX(void * arg, char * pdata, unsigned short len)
{
	struct espconn * T_arg = arg;		// �����������ӽṹ��ָ��

	remot_info * P_port_info = NULL;	// Զ��������Ϣ�ṹ��ָ��

	// ������������LED����/��
	//-------------------------------------------------------------------------------
	if(pdata[0] == 'k' || pdata[0] == 'K')	LED_ON;			// ����ĸΪ'k'/'K'������
	else if(pdata[0] == 'g' || pdata[0] == 'G')	LED_OFF;	// ����ĸΪ'g'/'G'������
	os_printf("\nESP8266_Receive_Data = %s\n",pdata);		// ���ڴ�ӡ���յ�������


	// ��ȡԶ����Ϣ��UDPͨ���������ӵģ���Զ��������Ӧʱ���ȡ�Է���IP/�˿���Ϣ��
	//------------------------------------------------------------------------------------
	if(espconn_get_connection_info(T_arg, &P_port_info, 0)==ESPCONN_OK)	// ��ȡԶ����Ϣ
	{
		T_arg->proto.udp->remote_port  = P_port_info->remote_port;		// ��ȡ�Է��˿ں�
		T_arg->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// ��ȡ�Է�IP��ַ
		T_arg->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		T_arg->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		T_arg->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];
		//os_memcpy(T_arg->proto.udp->remote_ip,P_port_info->remote_ip,4);	// �ڴ濽��
	}

	//--------------------------------------------------------------------
	OLED_ShowIP(24,6,T_arg->proto.udp->remote_ip);	// ��ʾԶ������IP��ַ
	//--------------------------------------------------------------------

	espconn_send(T_arg,"ESP8266_WIFI_Recv_OK",os_strlen("ESP8266_WIFI_Recv_OK"));	// ��Է�����Ӧ��
}
//=========================================================================================================


// �٣�����espconn�ͽṹ��(�������ӽṹ��)
//------------------------------------------------------------------------------
struct espconn ST_NetCon;	// ע�����붨��Ϊȫ�ֱ������ں˽���ʹ�ô˱���(�ڴ�)

//esp_udp ST_UDP;	// UDPͨ�Žṹ��

// ��ʼ����������(UDPͨ��)
//=================================================================================================
void ICACHE_FLASH_ATTR ESP8266_NetCon_Init_JX()
{
	// �ڣ��ṹ�帳ֵ
	//��������������������������������������������������������������������������
	ST_NetCon.type = ESPCONN_UDP;		// ͨ��Э�飺UDP

	// ST_NetCon.proto.udpֻ��һ��ָ�룬����������esp_udp�ͽṹ�����
	//-------------------------------------------------------------------------
	ST_NetCon.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));	// �����ڴ�
	//ST_NetCon.proto.udp = &ST_UDP;

	// �˴���������Ŀ��IP/�˿�(ESP8266��ΪServer������ҪԤ��֪��Client��IP/�˿�)
	//--------------------------------------------------------------------------
	//ST_NetCon.proto.udp->local_port  = espconn_port();	// ��ȡ8266���ö˿�
	ST_NetCon.proto.udp->local_port  = 8266 ;		// ���ñ��ض˿�
	//ST_NetCon.proto.udp->remote_port = 8888;		// ����Ŀ��˿�
	//ST_NetCon.proto.udp->remote_ip[0] = 192;		// ����Ŀ��IP��ַ
	//ST_NetCon.proto.udp->remote_ip[1] = 168;
	//ST_NetCon.proto.udp->remote_ip[2] = 4;
	//ST_NetCon.proto.udp->remote_ip[3] = 2;
	//��������������������������������������������������������������������������

	// �ۣ�ע��/����ص�����
	//-------------------------------------------------------------------------------------------
	espconn_regist_sentcb(&ST_NetCon,ESP8266_WIFI_Send_Cb_JX);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(&ST_NetCon,ESP8266_WIFI_Recv_Cb_JX);	// ע���������ݽ��ճɹ��Ļص�����


	// �ܣ�����UDP��ʼ��API
	//----------------------------------------------
	espconn_create(&ST_NetCon);	// ��ʼ��UDPͨ��
}
//=================================================================================================


// ��ʱ�Ļص�����(��ѯESP8266��IP��ַ������ʼ����������)
//=============================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	struct ip_info ST_ESP8266_IP;	// IP��Ϣ�ṹ��

	u8  ESP8266_IP[4];		// ���ʮ������ʽ����IP


	wifi_get_ip_info(SOFTAP_IF,&ST_ESP8266_IP);	// ��ѯAPģʽ��ESP8266��IP��ַ

	if(ST_ESP8266_IP.ip.addr != 0)				// ESP8266�ɹ���ȡ��IP��ַ
	{
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// ���ʮ����IP�ĵ�һ���� <==> addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// ���ʮ����IP�ĵڶ����� <==> addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// ���ʮ����IP�ĵ������� <==> addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// ���ʮ����IP�ĵ��ĸ��� <==> addr�߰�λ

		// ��ӡ����ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(24,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------


		os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��

		ESP8266_NetCon_Init_JX();		// ��ʼ����������(UDPͨ��)
	}
}
//=============================================================================================================


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
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");

	// OLED��ʾ��ʼ��
	//--------------------------------------------------------
	OLED_Init();							// OLED��ʼ��
	OLED_ShowString(0,0,"ESP8266 = AP");	// ESP8266ģʽ
	OLED_ShowString(0,2,"IP:");				// ESP8266_IP��ַ
	OLED_ShowString(0,4,"Remote  = STA");	// Զ������ģʽ
	OLED_ShowString(0,6,"IP:");				// Զ������IP��ַ
	//--------------------------------------------------------

	LED_Init_JX();		// LED��ʼ��


	ESP8266_AP_Init_JX();			// ��ʼ��ESP8266_APģʽ

	OS_Timer_1_Init_JX(1000,1);		// 1�������ʱ
}
//==============================================================================

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
