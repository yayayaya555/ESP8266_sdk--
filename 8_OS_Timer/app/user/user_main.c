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
// ���̣�	OS_Timer									//
//														//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//
//														//
// ���ܣ�	�٣���ʼ��LED(GPIO4)						//
//														//
//			�ڣ���ʼ�������ʱ��(500ms)					//
//														//
//			�ۣ�LEDÿ500Ms��תһ��						//
//														//
// �汾��	V1.0										//
//														//
//////////////////////////////////////////////////////////


// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����

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
#define		ProjectName		"OS_Timer"		// �������궨��
//==================================================================================


// ȫ�ֱ���
//==================================================================================
u8 F_LED = 0;				// LED״̬��־λ

// os_timer_t OS_Timer_1;	// �����ʱ��
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ע��OS_Timer_1���붨��Ϊȫ�ֱ�������ΪESP8266���ں˻�Ҫʹ��
//--------------------------------------------------------------------
os_timer_t OS_Timer_1;	// �٣����������ʱ��(os_timer_t�ͽṹ��)

// �����ʱ�Ļص�����
//======================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)		// �ڣ�����ص�����
{
	F_LED = !F_LED;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);		// LED״̬��ת


	os_printf("\r\n----OS_Timer_1_cb----\r\n");	// ����ص�������־
}
//======================================================================


// �����ʱ����ʼ��(ms����)
//================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	// �رն�ʱ��
	// ����һ��Ҫ�رյĶ�ʱ��
	//--------------------------------------------------------
	os_timer_disarm(&OS_Timer_1);	// �ۣ��ر������ʱ��


	// ���ö�ʱ��
	// ����һ��Ҫ���õĶ�ʱ�������������ص�����(������ת��)�����������ص������Ĳ���
	//��ע��os_timer_setfn�����������ʱ��δʹ�ܵ�����µ��á�
	//------------------------------------------------------------------------------------------
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// �ܣ����ûص�����


	// ʹ��(����)ms��ʱ��
	// ����һ��Ҫʹ�ܵĶ�ʱ��������������ʱʱ�䣨��λ��ms������������1=�ظ�/0=ֻһ��
	//------------------------------------------------------------------------------------------
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // �ݣ����ö�ʱ��������ʹ�ܶ�ʱ��
	//-------------------------------------------------------------------
	// ����δ����system_timer_reinit����֧�ַ�Χ��[5ms �� 6,870,947ms]��
	// ���������system_timer_reinit����֧�ַ�Χ��[100ms �� 428,496 ms]��
	//-------------------------------------------------------------------
}
//================================================================================================

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


	LED_Init_JX();					// LED��ʼ��


	// ��ʼ�������ʱ��
	//--------------------------------------------
	OS_Timer_1_Init_JX(500,1);		// 500ms(�ظ�)



//	while(1) system_soft_wdt_feed();	// ��ѭ����������

	os_printf("\r\n-------------------- user_init OVER --------------------\r\n");
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
