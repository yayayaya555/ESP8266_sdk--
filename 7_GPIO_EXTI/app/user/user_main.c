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
// ���̣�	GPIO_EXTI									//
//														//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//
//														//
// ���ܣ�	�٣���ʼ��LED(GPIO4)						//
//														//
//			�ڣ���ʼ��������GPIO0��Ϊ���룬�ⲿ����		//
//														//
//			�ۣ���ʼ��GPIO_0�ж�						//
//														//
//			�ܣ��ȴ��������½����ж�					//
//														//
//			�ݣ��������º�LED״̬��ת(δ������)		//
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
#define		ProjectName		"GPIO_EXTI"			// �������궨��
//==================================================================================


// ȫ�ֱ���
//==================================================================================
u8 F_LED = 0;		// LED״̬��־λ
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// GPIO�жϺ�����ע�⣺�жϺ���ǰ��Ҫ��"ICACHE_FLASH_ATTR"�꡿
//=============================================================================
void GPIO_INTERRUPT(void)
{
	u32	S_GPIO_INT;		// ����IO�ڵ��ж�״̬
	u32 F_GPIO_0_INT;	// GPIO_0���ж�״̬

	// ��ȡGPIO�ж�״̬
	//---------------------------------------------------
	S_GPIO_INT = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	// ����ж�״̬λ(��������״̬λ�������������ж�)
	//----------------------------------------------------
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, S_GPIO_INT);


	F_GPIO_0_INT = S_GPIO_INT & (0x01<<0);	// ��ȡGPIO_0�ж�״̬

	// �ж��Ƿ���KEY�ж�(δ������)
	//------------------------------------------------------------
	if(F_GPIO_0_INT)	// GPIO_0���½����ж�
	{
		F_LED = !F_LED;

		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);	// LED״̬��ת
	}
}
//=============================================================================


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


	// ��ʼ��LED(ע�⡾PIN_NAME������FUNC������gpio_no����Ҫ����)
	//-------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO_4��ΪIO��
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// GPIO_4 = 1


	// ��ʼ������(BOOT == GPIO0)
	//-----------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,	FUNC_GPIO0);	// GPIO_0��ΪGPIO��
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));						// GPIO_0ʧ�����(Ĭ��)
//	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0ʧ������(Ĭ��)
//	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0ʹ������


	// GPIO_0�ж�����
	//----------------------------------------------------------------------------------
	ETS_GPIO_INTR_DISABLE();										// �ر�GPIO�жϹ���
	ETS_GPIO_INTR_ATTACH((ets_isr_t)GPIO_INTERRUPT,NULL);			// ע���жϻص�����
	gpio_pin_intr_state_set(GPIO_ID_PIN(0),GPIO_PIN_INTR_NEGEDGE);	// GPIO_0�½����ж�
	ETS_GPIO_INTR_ENABLE();											// ��GPIO�жϹ���

	//��������������������������������������������
    // GPIO_PIN_INTR_DISABLE = 0,	// �������ж�
    // GPIO_PIN_INTR_POSEDGE = 1,	// �������ж�
    // GPIO_PIN_INTR_NEGEDGE = 2,	// �½����ж�
    // GPIO_PIN_INTR_ANYEDGE = 3,	// ˫�����ж�
    // GPIO_PIN_INTR_LOLEVEL = 4,	// �͵�ƽ�ж�
    // GPIO_PIN_INTR_HILEVEL = 5	// �ߵ�ƽ�ж�
	//��������������������������������������������
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
