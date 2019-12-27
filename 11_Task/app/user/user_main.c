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

//////////////////////////////////////////////////////
//													//
// ���̣�	Task									//
//													//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0	//
//													//
// ���ܣ�	�� Ϊϵͳ��������						//
//													//
//			�� Ϊϵͳ��������						//
//													//
// �汾��	V1.0									//
//													//
//////////////////////////////////////////////////////


// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����

//#include "at_custom.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ip_addr.h"			// ��"espconn.h"ʹ��
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
#define		ProjectName			"Task"		// �������궨��

#define		MESSAGE_QUEUE_LEN	2			// ��Ϣ�������(����ͬһ������ϵͳ�����ܵĵ���������)
//==================================================================================

// ȫ�ֱ���
//=========================================================
os_event_t * Pointer_Task_1 ;	// ��������1(���ڴ�ӡ����)		// �ڢٲ�����������ָ��
//=========================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ����ִ�к���(�βΣ����ͱ���Ϊ��os_event_t *��)		// �ڢ۲�������������
//======================================================================================
void Func_Task_1(os_event_t * Task_message)	// Task_message = Pointer_Task_1
{
	// Task_message->sig=��Ϣ���͡�Task_message->par=��Ϣ����	// �ڢ޲�����д������(������Ϣ����/��Ϣ����ʵ����Ӧ����)
	//--------------------------------------------------------------------------------

	os_printf("��Ϣ����=%d����Ϣ����=%c\r\n",Task_message->sig, Task_message->par);

}
//======================================================================================


// user_init��entry of user application, init user function here
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	u8 C_Task = 0 ;			// �����������
	u8 Message_Type = 1;	// ��Ϣ����
	u8 Message_Para = 'A';	// ��Ϣ����


	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// ������1����ռ�(����1�ռ� = 1�����пռ� * ������)	// �ڢڲ���Ϊ��������ڴ�
	//-------------------------------------------------------------------------------
	Pointer_Task_1=(os_event_t*)os_malloc((sizeof(os_event_t))*MESSAGE_QUEUE_LEN);

	// ��������(����1=����ִ�к��� / ����2=����ȼ� / ����3=����ռ�ָ�� / ����4=��Ϣ�������)	// �ڢܲ�����������
	//-----------------------------------------------------------------------------------------
	system_os_task(Func_Task_1, USER_TASK_PRIO_0, Pointer_Task_1, MESSAGE_QUEUE_LEN);


	// ����4������
	//--------------------------------
	for(C_Task=1; C_Task<=4; C_Task++)
	{
		system_soft_wdt_feed();	// ι������ֹ��λ

		delay_ms(1000);			// ��ʱ1��
		os_printf("\r\n��������Task == %d\r\n",C_Task);

		// ��������(����1=����ȼ� / ����2=��Ϣ���� / ����3=��Ϣ����)
		// ע�⣺����3����Ϊ�޷���������������Ҫǿ������ת��
		//---------------------------------------------------------------
		system_os_post(USER_TASK_PRIO_0, Message_Type++, Message_Para++);	// �ڢݲ�����ϵͳ��������
	}

	os_printf("\r\n------------------ user_init OVER ----------------\r\n\r\n");
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
