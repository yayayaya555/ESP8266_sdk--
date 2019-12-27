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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																	//													//
// ���̣�	HTTP													//	�٣��޸�WIFI����WIFI���룬����WIFI				//
//																	//													//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0					//	�ڣ���λ8266���ȴ�8266����WIFI					//
//																	//													//
// ���ܣ�	�٣�8266����ΪSTAģʽ������·����WIFI					//	�ۣ��򿪡�www.rationmcu.com/elecjc/2397.html��	//
//																	//													//
//			�ڣ�����TCPͨ�Ų���										//	�ܣ�����Ҽ���ѡ�񡾲鿴Դ���롿				//
//																	//													//
//			�ۣ���������"www.rationmcu.com"����ȡ������IP��ַ		//	�ݣ��Աȣ�8266�յ�����ҳ��Ϣ����ҳԴ������Ϣ	//
//																	//													//
//			�ܣ�ESP8266��ΪTCP_Client������"www.rationmcu.com"		//	�ޣ��������һ�£�˵��HTTPͨ��(��ҳ��ȡ)�ɹ�	//
//																	//													//
//			�ݣ���Server����HTTPЭ�鱨�ģ���ȡ��ҳ��Ϣ				//	�ߣ���ע�⣺��ҳĬ�ϱ��뷽��UTF-8��				//
//																	//													//
//			�ޣ���ӡ��www.rationmcu.com/elecjc/2397.html����ҳ��Ϣ	//	�ࣺ���ڵ�������һ����ASCII/GBK/GB2312����		//
//																	//													//
//	�汾��	V1.0													//	�᣺���������յ��������ַ�Ϊ��������������		//
//																	//													//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
//=========================================================================================================
#define		ProjectName			"HTTP"					// �������궨��

#define		ESP8266_STA_SSID	"JI-XIN-TEC"			// �����WIFI��
#define		ESP8266_STA_PASS	"JIXINDIANZI78963"		// �����WIFI����

#define 	DN_Server			"www.rationmcu.com"		// ��������������


#define		LED_ON				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0)		// LED��
#define		LED_OFF				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1)		// LED��


//#define 	HTTP_Message_485Comm	"GET http://www.rationmcu.com/elecjc/2397.html HTTP/1.1\r\nHost:www.rationmcu.com\r\nConnection:close\r\n\r\n"

#define 	HTTP_Message_485Comm	"GET http://www.rationmcu.com/elecjc/2397.html HTTP/1.1  \r\n"		\
																										\
									"Host:www.rationmcu.com  \r\n"										\
																										\
									"Connection:close  \r\n\r\n"
//=========================================================================================================


// ȫ�ֱ���
//==================================================================================
os_timer_t OS_Timer_IP;			// ���������ʱ��

struct espconn ST_NetCon;		// �������ӽṹ��

ip_addr_t IP_Server;			// IP��ַ�ͽṹ�塾32λ������IP��ַ_��������
//==================================================================================



// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ESP8266_STA��ʼ��
//=========================================================================================
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
	IP4_ADDR(&ST_ESP8266_IP.ip,192,168,8,82);		// ����IP��ַ
	IP4_ADDR(&ST_ESP8266_IP.gw,192,168,8,1);		// �������ص�ַ
	IP4_ADDR(&ST_ESP8266_IP.netmask,255,255,255,0);	// ������������
	wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����STAģʽ�µ�IP��ַ
	*/

	// �ṹ�帳ֵ������STAģʽ����
	//-------------------------------------------------------------------------------
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA�����ṹ�� = 0
	os_strcpy(STA_Config.ssid,ESP8266_STA_SSID);				// ����WIFI��
	os_strcpy(STA_Config.password,ESP8266_STA_PASS);			// ����WIFI����

	wifi_station_set_config(&STA_Config);	// ����STA�����������浽Flash

	// wifi_station_connect();		// ESP8266����WIFI
}
//=========================================================================================


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
	os_printf("\nESP8266_Receive_Data = %s\n",pdata);		// ���ڴ�ӡ���յ�������
}
//=========================================================================================================


// TCP���ӶϿ��ɹ��Ļص�����
//================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Disconnect_Cb_JX(void *arg)
{
	os_printf("\nESP8266_TCP_Disconnect_OK\n");
}
//================================================================


// TCP���ӽ����ɹ��Ļص�����
//====================================================================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Connect_Cb_JX(void *arg)
{
	espconn_regist_sentcb((struct espconn *)arg, ESP8266_WIFI_Send_Cb_JX);			// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb((struct espconn *)arg, ESP8266_WIFI_Recv_Cb_JX);			// ע���������ݽ��ճɹ��Ļص�����
	espconn_regist_disconcb((struct espconn *)arg,ESP8266_TCP_Disconnect_Cb_JX);	// ע��ɹ��Ͽ�TCP���ӵĻص�����

	os_printf("\r\n--------------- ESP8266_TCP_Connect_OK ---------------\r\n");

	espconn_send((struct espconn *)arg, HTTP_Message_485Comm, os_strlen(HTTP_Message_485Comm));		// ����HTTP����
}
//====================================================================================================================


// TCP�����쳣�Ͽ�ʱ�Ļص�����
//====================================================================
void ICACHE_FLASH_ATTR ESP8266_TCP_Break_Cb_JX(void *arg,sint8 err)
{
	os_printf("\nESP8266_TCP_Break\n");

	espconn_connect(&ST_NetCon);	// ����TCP-server
}
//====================================================================


// DNS_������������_�ص�����������1�������ַ���ָ�� / ����2��IP��ַ�ṹ��ָ�� / ����3���������ӽṹ��ָ�롿
//=========================================================================================================
void ICACHE_FLASH_ATTR DNS_Over_Cb_JX(const char * name, ip_addr_t *ipaddr, void *arg)
{
	struct espconn * T_arg = (struct espconn *)arg;	// �����������ӽṹ��ָ��

	//������������������������������������������������������������
	if(ipaddr == NULL)		// ��������ʧ��
	{
		os_printf("\r\n---- DomainName Analyse Failed ----\r\n");

		return;
	}

	//����������������������������������������������������������������������������������
	else if (ipaddr != NULL && ipaddr->addr != 0)		// ���������ɹ�
	{
		os_printf("\r\n---- DomainName Analyse Succeed ----\r\n");

		IP_Server.addr = ipaddr->addr;					// ��ȡ������IP��ַ


		// ���������ķ�����IP��ַ��ΪTCP���ӵ�Զ��IP��ַ
		//------------------------------------------------------------------------------
		os_memcpy(T_arg->proto.tcp->remote_ip, &IP_Server.addr, 4);	// ���÷�����IP��ַ


		// ��ʾ������������IP��ַ
		//------------------------------------------------------------------------------
		os_printf("\r\nIP_Server = %d.%d.%d.%d\r\n",						// ���ڴ�ӡ
				*((u8*)&IP_Server.addr),	*((u8*)&IP_Server.addr+1),
				*((u8*)&IP_Server.addr+2),	*((u8*)&IP_Server.addr+3));
		OLED_ShowIP(24,6,T_arg->proto.tcp->remote_ip);						// OLED��ʾ
		//------------------------------------------------------------------------------

		// ע�����ӳɹ��ص��������쳣�Ͽ��ص�����
		//----------------------------------------------------------------------------------------------
		//espconn_regist_connectcb(T_arg, ESP8266_TCP_Connect_Cb_JX);	// ע��TCP���ӳɹ������Ļص�����
		//espconn_regist_reconcb(T_arg, ESP8266_TCP_Break_Cb_JX);		// ע��TCP�����쳣�Ͽ��Ļص�����


		// ���� TCP server
		//----------------------------------------------------------
		espconn_connect(T_arg);	// ����TCP-server
	}
}
//=========================================================================================================



// ����espconn�ͽṹ��
//-----------------------------------------------
//struct espconn ST_NetCon;	// �������ӽṹ��

// ��ʼ����������(TCPͨ��)
//=========================================================================================================
void ICACHE_FLASH_ATTR ESP8266_NetCon_Init_JX()
{
	// �ṹ�帳ֵ
	//--------------------------------------------------------------------------
	ST_NetCon.type = ESPCONN_TCP ;				// ����ΪTCPЭ��

	ST_NetCon.proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));	// �����ڴ�


	// ESP8266��ΪTCP_Client����Ҫ����TCP_Server������֪��TCP_Server��IP��ַ
	//--------------------------------------------------------------------------------
	ST_NetCon.proto.tcp->local_port = espconn_port() ;	// ���ض˿ڡ���ȡ���ö˿ڡ�
	ST_NetCon.proto.tcp->remote_port = 80;				// Ŀ��˿ڡ�HTTP�˿ں�80��
	//u8 remote_ip[4] = {121,198,34,91};				// Ŀ��IP��www.rationmcu.com��
	//os_memcpy(ST_NetCon.proto.udp->remote_ip,remote_ip,4);

	// ��ȡ��������Ӧ��IP��ַ
	//������1���������ӽṹ��ָ�� / ����2�������ַ��� / ����3��IP��ַ�ṹ��ָ�� / ����4���ص�������
	//---------------------------------------------------------------------------------------------
	espconn_gethostbyname(&ST_NetCon, DN_Server, &IP_Server, DNS_Over_Cb_JX);


	// ע�����ӳɹ��ص��������쳣�Ͽ��ص�����
	//--------------------------------------------------------------------------------------------------
	espconn_regist_connectcb(&ST_NetCon, ESP8266_TCP_Connect_Cb_JX);	// ע��TCP���ӳɹ������Ļص�����
	espconn_regist_reconcb(&ST_NetCon, ESP8266_TCP_Break_Cb_JX);		// ע��TCP�����쳣�Ͽ��Ļص�����


	// ���� TCP server
	//----------------------------------------------------------
	//espconn_connect(&ST_NetCon);	// ����TCP-server
}
//=========================================================================================================


// �����ʱ�Ļص�����
//=========================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_IP_cb(void)
{
	u8 C_LED_Flash = 0;				// LED��˸�ƴ�

	struct ip_info ST_ESP8266_IP;	// ESP8266��IP��Ϣ
	u8 ESP8266_IP[4];				// ESP8266��IP��ַ


	// �ɹ�����WIFI��STAģʽ�£��������DHCP(Ĭ��)����ESO8266��IP��ַ��WIFI·�����Զ����䡿
	//-------------------------------------------------------------------------------------
	if( wifi_station_get_connect_status() == STATION_GOT_IP )	// �ж��Ƿ��ȡIP
	{
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ��ȡSTA��IP��Ϣ
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;			// IP��ַ�߰�λ == addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;		// IP��ַ�θ߰�λ == addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;		// IP��ַ�εͰ�λ == addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;		// IP��ַ�Ͱ�λ == addr�߰�λ

		// ��ʾESP8266��IP��ַ
		//------------------------------------------------------------------------------------------------
		os_printf("\nESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(24,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
		//------------------------------------------------------------------------------------------------

		// ����WIFI�ɹ���LED����3��
		//----------------------------------------------------
		for(; C_LED_Flash<=5; C_LED_Flash++)
		{
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),(C_LED_Flash%2));
			delay_ms(100);
		}


		os_timer_disarm(&OS_Timer_IP);	// �رն�ʱ��

		ESP8266_NetCon_Init_JX();		// ��ʼ����������(TCPͨ��)
	}
}
//=========================================================================================================

// �����ʱ����ʼ��(ms����)
//=====================================================================================
void ICACHE_FLASH_ATTR OS_Timer_IP_Init_JX(u32 time_ms, u8 time_repetitive)
{
	os_timer_disarm(&OS_Timer_IP);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_IP,(os_timer_func_t *)OS_Timer_IP_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_IP, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
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
	//------------------------------------------------------------
	OLED_Init();							// OLED��ʼ��
	OLED_ShowString(0,0,"ESP8266-STA");		// ESP8266-STA
	OLED_ShowString(0,2,"IP:");				// ESP8266_IP
	OLED_ShowString(0,4,"rationmcu.com");	// www.rationmcu.com
	OLED_ShowString(0,6,"IP:");				// ������_IP
	//------------------------------------------------------------

	LED_Init_JX();		// LED��ʼ��


	ESP8266_STA_Init_JX();			// ESP8266_STA��ʼ��

	OS_Timer_IP_Init_JX(1000,1);	// 1���ظ���ʱ(��ȡIP��ַ)
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
