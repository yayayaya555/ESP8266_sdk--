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

//////////////////////////////////////////////////////////////////
//																//
// ���̣�	Flash												//
//																//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0				//
//																//
// ���ܣ�	�� ���ⲿFlash��0x77 000����ַ��д��"1��16"		//
//																//
//			�� ���ⲿFlash��0x77 000����ַ�𣬶���16������		//
//																//
// �汾��	V1.0												//
//																//
//////////////////////////////////////////////////////////////////


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
#define		ProjectName			"Flash"		// �������궨��

#define		SPI_FLASH_SEC_SIZE	4096		// Flash������С
//==================================================================================

// ȫ�ֱ���
//==================================================================================
u16 N_Data_FLASH_SEC = 0x77;	// �洢���ݵ��������

u32 A_W_Data[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};	// д��Flash������

u32 A_R_Data[16] = {0};			// �����Flash������
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// user_init��entry of user application, init user function here
//==========================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	u8 C_loop = 0;

	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// ��0x77 000����ַ��д��16������(ÿ������ռ4�ֽ�)
	//��������������������������������������������������������������������������
	spi_flash_erase_sector(0x77);	// ����0x77����		����==��������š�

	// ��Flashд����(����1=���ֽڵ�ַ��������2=д�����ݵ�ָ�롢����3=���ݳ���)
	//------------------------------------------------------------------------
	spi_flash_write(0x77*4096, (uint32 *)A_W_Data, sizeof(A_W_Data));

	os_printf("\r\n---------- Write Flash Data OVER ----------\r\n");
	//��������������������������������������������������������������������������


	// �ӡ�0x77 000����ַ�𣬶���16������(ÿ������ռ4�ֽ�)
	//---------------------------------------------------------------------
	spi_flash_read(0x77*4096, (uint32 *)A_R_Data, sizeof(A_W_Data));


	// ���ڴ�ӡ����������
	//-----------------------------------------------------
	for(C_loop=0; C_loop<16; C_loop++)
	{
		os_printf("Read Data = %d \r\n",A_R_Data[C_loop]);

		delay_ms(10);
	}

	os_printf("\r\n\r\n------------ user_init OVER ------------\r\n\r\n");
}
//==========================================================================


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
