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

//////////////////////////////////////////////////////////
//														//
// ���̣�	DHT11										//
//														//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//
//														//
// ���ܣ�	�٣���ʼ��LED�������ʱ��					//
//														//
//			�ڣ�3���ظ���ʱ								//
//														//
//			�ۣ�ÿ3���ȡһ��DHT11��ʪ��				//
//														//
//			�ܣ��¶ȳ���30�棬LED��						//
//														//
//			�ݣ����ڡ�OLED��ʾ��ʪ��					//
//														//
// �汾��	V1.0										//
//														//
//////////////////////////////////////////////////////////


// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED
#include "driver/dht11.h"		// DHT11ͷ�ļ�

//#include "at_custom.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
//#include "espconn.h"
//#include "espnow.h"
#include "ets_sys.h"			// �ص�����
//#include "gpio.h"				//
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
#define		ProjectName			"DHT11"			// �������궨��
//==================================================================================


// ȫ�ֱ���
//==================================================================================
u8 F_LED = 0;				// LED״̬��־λ

os_timer_t OS_Timer_1;		// �����ʱ��
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ��ʱ�Ļص�����
//==========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	if(DHT11_Read_Data_Complete() == 0)		// ��ȡDHT11��ʪ��ֵ
	{
		//-------------------------------------------------
		// DHT11_Data_Array[0] == ʪ��_����_����
		// DHT11_Data_Array[1] == ʪ��_С��_����
		// DHT11_Data_Array[2] == �¶�_����_����
		// DHT11_Data_Array[3] == �¶�_С��_����
		// DHT11_Data_Array[4] == У���ֽ�
		// DHT11_Data_Array[5] == ��1:�¶�>=0����0:�¶�<0��
		//-------------------------------------------------


		// �¶ȳ���30�棬LED��
		//----------------------------------------------------
		if(DHT11_Data_Array[5]==1 && DHT11_Data_Array[2]>=30)
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);		// LED��
		else
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);		// LED��


		// ���������ʪ��
		//---------------------------------------------------------------------------------
		if(DHT11_Data_Array[5] == 1)			// �¶� >= 0��
		{
			os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
			os_printf("\r\n�¶� == %d.%d ��\r\n", DHT11_Data_Array[2],DHT11_Data_Array[3]);
		}
		else // if(DHT11_Data_Array[5] == 0)	// �¶� < 0��
		{
			os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
			os_printf("\r\n�¶� == -%d.%d ��\r\n",DHT11_Data_Array[2],DHT11_Data_Array[3]);
		}

		// OLED��ʾ��ʪ��
		//---------------------------------------------------------------------------------
		DHT11_NUM_Char();	// DHT11����ֵת���ַ���

		OLED_ShowString(0,2,DHT11_Data_Char[0]);	// DHT11_Data_Char[0] == ��ʪ���ַ�����
		OLED_ShowString(0,6,DHT11_Data_Char[1]);	// DHT11_Data_Char[1] == ���¶��ַ�����
	}
}
//==========================================================================================

// �����ʱ����ʼ��(ms����)
//==========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//==========================================================================================

// LED��ʼ��
//==========================================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}
//==========================================================================================

// user_init��entry of user application, init user function here
//==========================================================================================
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
	OLED_ShowString(0,0,"Humidity:");		// ʪ��
	OLED_ShowString(0,4,"Temperature:");	// �¶�
	//--------------------------------------------------------

	LED_Init_JX();		// LED��ʼ��

	OS_Timer_1_Init_JX(3000,1);		// 3�붨ʱ(�ظ�)
}
//==========================================================================================


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
