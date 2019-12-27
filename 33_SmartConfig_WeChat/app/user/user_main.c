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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													//																				//
// ���̣�	SmartConfig_WeChat						// �٣��ֻ�Ԥ�Ƚ���8266��Ҫ���õ�WIFI��Ȼ��λESP8266							//
//													//																				//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0	// �ڣ�OLED��ʾ��WIFI Connecting................��								//
//													//																				//
// ���ܣ�	�٣�8266����ΪSTAģʽ������WIFI�ȵ�		// �ۣ������WIFI��/���롿����OLED��ʾ��Use WeChat to SmartConfig��			//
//													//																				//
//			�ڣ����WIFI���ӳ������롾΢��������	// �ܣ��򿪡����ſɿƼ���΢�Ź��ںţ������WIFI���á�							//
//													//																				//
//			�ۣ�8266��ȡ�ֻ����͵ġ�WIFI��/���롿	// �ݣ��ڡ�WIFI���á�ҳ�棬�������ʼ���á�										//
//													//																				//
//			�ܣ�ESP8266����WIFI(STA���� -> Flash)	// �ޣ��ڡ������豸������ҳ�棬�鿴��WIFI�����Ƿ���ȷ�������á�WIFI���롿		//
//													//																				//
//			�ݣ����ӳɹ���LED����3��				// �ߣ���������ӡ�����ʾ"������"��OLED��ʾ��Use WeChat to SmartConfig��		//
//													//																				//
//	�汾��	V1.0									// �ࣺWIFI���ӳɹ�����ʾ"���óɹ�"��OLED��ʾ��Connect to WIFI Successfully��	//
//													//																				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
#include "smartconfig.h"		// ��������
//#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro
//==================================================================================


// �궨��
//==================================================================================
#define		ProjectName			"SmartConfig_WeChat"	// �������궨��

#define		Sector_STA_INFO		0x80			// ��STA��������������
//==================================================================================

// ȫ�ֱ���
//==================================================================================
struct station_config STA_INFO;		// ��STA�������ṹ��

os_timer_t OS_Timer_IP;				// �����ʱ��

struct ip_info ST_ESP8266_IP;		// 8266��IP��Ϣ
u8 ESP8266_IP[4];					// 8266��IP��ַ

u8 C_LED_Flash = 0;					// LED��˸�ƴ�
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// SmartConfig״̬�����ı�ʱ������˻ص�����
//--------------------------------------------
// ����1��sc_status status / ����2��������ָ�롾�ڲ�ͬ״̬�£�[void *pdata]�Ĵ�������ǲ�ͬ�ġ�
//=================================================================================================================
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void *pdata)
{
	os_printf("\r\n------ smartconfig_done ------\r\n");	// ESP8266����״̬�ı�

    switch(status)
    {
    	// CmartConfig�ȴ�
    	//����������������������������������������
        case SC_STATUS_WAIT:		// ��ʼֵ
            os_printf("\r\nSC_STATUS_WAIT\r\n");
        break;
        //����������������������������������������

        // ���֡�WIFI�źš���8266������״̬�µȴ�������
        //��������������������������������������������������������������������������
        case SC_STATUS_FIND_CHANNEL:
            os_printf("\r\nSC_STATUS_FIND_CHANNEL\r\n");

    		os_printf("\r\n---- Please Use WeChat to SmartConfig ------\r\n\r\n");

    		OLED_ShowString(0,4,"Use WeChat to   ");
    		OLED_ShowString(0,6,"SmartConfig     ");
    	break;
    	//��������������������������������������������������������������������������

        // ���ڻ�ȡ��SSID����PSWD����8266����ץȡ�����ܡ�SSID+PSWD����
        //��������������������������������������������������������������������������
        case SC_STATUS_GETTING_SSID_PSWD:
            os_printf("\r\nSC_STATUS_GETTING_SSID_PSWD\r\n");

            // ��SC_STATUS_GETTING_SSID_PSWD��״̬�£�����2==SmartConfig����ָ��
            //-------------------------------------------------------------------
			sc_type *type = pdata;		// ��ȡ��SmartConfig���͡�ָ��

			// ������ʽ == ��ESPTOUCH��
			//-------------------------------------------------
            if (*type == SC_TYPE_ESPTOUCH)
            { os_printf("\r\nSC_TYPE:SC_TYPE_ESPTOUCH\r\n"); }

            // ������ʽ == ��AIRKISS��||��ESPTOUCH_AIRKISS��
            //-------------------------------------------------
            else
            { os_printf("\r\nSC_TYPE:SC_TYPE_AIRKISS\r\n"); }

	    break;
	    //��������������������������������������������������������������������������

        // �ɹ���ȡ����SSID����PSWD��������STA������������WIFI
	    //��������������������������������������������������������������������������
        case SC_STATUS_LINK:
            os_printf("\r\nSC_STATUS_LINK\r\n");

            // ��SC_STATUS_LINK��״̬�£�����2 == STA�����ṹ��ָ��
            //------------------------------------------------------------------
            struct station_config *sta_conf = pdata;	// ��ȡ��STA������ָ��

            // ����SSID����PASS�����浽���ⲿFlash����
            //--------------------------------------------------------------------------
			spi_flash_erase_sector(Sector_STA_INFO);						// ��������
			spi_flash_write(Sector_STA_INFO*4096, (uint32 *)sta_conf, 96);	// д������
			//--------------------------------------------------------------------------

			wifi_station_set_config(sta_conf);			// ����STA������Flash��
	        wifi_station_disconnect();					// �Ͽ�STA����
	        wifi_station_connect();						// ESP8266����WIFI

	    	OLED_ShowString(0,4,"WIFI Connecting ");	// OLED��ʾ��
	    	OLED_ShowString(0,6,"................");	// ��������WIFI

	    break;
	    //��������������������������������������������������������������������������


        // ESP8266��ΪSTA���ɹ����ӵ�WIFI
	    //��������������������������������������������������������������������������
        case SC_STATUS_LINK_OVER:
            os_printf("\r\nSC_STATUS_LINK_OVER\r\n");

            smartconfig_stop();		// ֹͣSmartConfig���ͷ��ڴ�

            //**************************************************************************************************
            wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ��ȡ8266_STA��IP��ַ

			ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// IP��ַ�߰�λ == addr�Ͱ�λ
			ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// IP��ַ�θ߰�λ == addr�εͰ�λ
			ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// IP��ַ�εͰ�λ == addr�θ߰�λ
			ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// IP��ַ�Ͱ�λ == addr�߰�λ

			// ��ʾESP8266��IP��ַ
			//-----------------------------------------------------------------------------------------------
			os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
			OLED_ShowIP(24,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
			OLED_ShowString(0,4,"Connect to WIFI ");
			OLED_ShowString(0,6,"Successfully    ");
			//-----------------------------------------------------------------------------------------------

			// ����WIFI�ɹ���LED����3��
			//----------------------------------------------------
			for(; C_LED_Flash<=5; C_LED_Flash++)
			{
				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),(C_LED_Flash%2));
				delay_ms(100);
			}

			os_printf("\r\n---- ESP8266 Connect to WIFI Successfully ----\r\n");

			//*****************************************************
			// WIFI���ӳɹ���ִ�к������ܡ�	�磺SNTP/UDP/TCP/DNS��
			//*****************************************************

			//**************************************************************************************************

	    break;
	    //��������������������������������������������������������������������������

    }
}
//=================================================================================================================


// IP��ʱ���Ļص�����
//========================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_IP_cb(void)
{
	u8 S_WIFI_STA_Connect;			// WIFI����״̬��־


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


	// �ɹ�����WIFI
	//----------------------------------------------------------------------------------------
	if( S_WIFI_STA_Connect == STATION_GOT_IP )	// �ж��Ƿ��ȡIP
	{
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ��ȡ8266_STA��IP��ַ

		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// IP��ַ�߰�λ == addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// IP��ַ�θ߰�λ == addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// IP��ַ�εͰ�λ == addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// IP��ַ�Ͱ�λ == addr�߰�λ

		// ��ʾESP8266��IP��ַ
		//-----------------------------------------------------------------------------------------------
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(24,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
		OLED_ShowString(0,4,"Connect to WIFI ");
		OLED_ShowString(0,6,"Successfully    ");
		//-----------------------------------------------------------------------------------------------

		// ����WIFI�ɹ���LED����3��
		//----------------------------------------------------
		for(; C_LED_Flash<=5; C_LED_Flash++)
		{
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),(C_LED_Flash%2));
			delay_ms(100);
		}

		os_printf("\r\n---- ESP8266 Connect to WIFI Successfully ----\r\n");

		os_timer_disarm(&OS_Timer_IP);	// �رն�ʱ��

		//*****************************************************
		// WIFI���ӳɹ���ִ�к������ܡ�	�磺SNTP/UDP/TCP/DNS��
		//*****************************************************
	}

	// ESP8266�޷�����WIFI
	//------------------------------------------------------------------------------------------------
	else if(	S_WIFI_STA_Connect==STATION_NO_AP_FOUND 	||		// δ�ҵ�ָ��WIFI
				S_WIFI_STA_Connect==STATION_WRONG_PASSWORD 	||		// WIFI�������
				S_WIFI_STA_Connect==STATION_CONNECT_FAIL		)	// ����WIFIʧ��
	{
		os_timer_disarm(&OS_Timer_IP);			// �رն�ʱ��

		os_printf("\r\n---- S_WIFI_STA_Connect=%d-----------\r\n",S_WIFI_STA_Connect);
		os_printf("\r\n---- ESP8266 Can't Connect to WIFI-----------\r\n");

		// ΢��������������
		//��������������������������������������������������������������������������������������������
		//wifi_set_opmode(STATION_MODE);			// ��ΪSTAģʽ						//���ڢٲ���

		smartconfig_set_type(SC_TYPE_AIRKISS); 	// ESP8266������ʽ��AIRKISS��			//���ڢڲ���

		smartconfig_start(smartconfig_done);	// ���롾��������ģʽ��,�����ûص�����	//���ڢ۲���
		//��������������������������������������������������������������������������������������������
	}
}
//========================================================================================================

// �����ʱ����ʼ��(ms����)
//========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_IP_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_IP);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_IP,(os_timer_func_t *)OS_Timer_IP_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_IP, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//========================================================================================



// LED��ʼ��
//=============================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}
//=============================================================================


// user_init��entry of user application, init user function here
//=================================================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// OLED��ʼ��
	//����������������������������������������������
	OLED_Init();								// |
	OLED_ShowString(0,0,"ESP8266 = STA");		// |
	OLED_ShowString(0,2,"IP:");					// |
	OLED_ShowString(0,4,"WIFI Connecting ");	// |
	OLED_ShowString(0,6,"................");	// |
	//����������������������������������������������

	LED_Init_JX();		// LED��ʼ��


// ESP8266��ȡ���ⲿFlash���еġ�STA������(SSID/PASS)����ΪSTA������WIFI
//������������������������������������������������������������������������������������������������
	os_memset(&STA_INFO,0,sizeof(struct station_config));			// STA_INFO = 0
	spi_flash_read(Sector_STA_INFO*4096,(uint32 *)&STA_INFO, 96);	// ������STA������(SSID/PASS)
	STA_INFO.ssid[31] = 0;		// SSID�����'\0'
	STA_INFO.password[63] = 0;	// APSS�����'\0'
	os_printf("\r\nSTA_INFO.ssid=%s\r\nSTA_INFO.password=%s\r\n",STA_INFO.ssid,STA_INFO.password);

	wifi_set_opmode(0x01);					// ����ΪSTAģʽ�������浽Flash
	wifi_station_set_config(&STA_INFO);		// ����STA����
	wifi_station_connect();					// ESP8266����WIFI������˾��ʡ��
//������������������������������������������������������������������������������������������������

	OS_Timer_IP_Init_JX(1000,1);	// ��ʱ��ѯ8266����WIFI���
}
//=================================================================================================


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
