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
// ���̣�	IIC_OLED								//	ע�⣺																		//
//													//																				//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0	//	0.96'OLED�ֱ��� == 128*64		����ÿһ����128���㣬ÿһ����64����			//
//													//																				//
// ���ܣ�	�٣���ʼ��OLED							//	�ַ��Ĵ�СΪ��8*16��			����һ���ַ�ռ������8���㡢����16���㡿		//
//													//																				//
//			�ڣ���ʾ��Project=IIC_OLED��			//	ֻ֧��ASCII����У��ӡ�' '��~��'~'�����ַ�			ע���������޸�			//
//													//																				//
//			�ۣ���ʾ��IP:192.168.4.1��				//	��OLED_ShowString(x, y, "...")����ʾ�ַ�/�ַ���		ע����x��y��0��ʼ��		//
//													//																				//
//			�ܣ���ʾ��0123456789ABCDEFGHIJKLMN��	//	��OLED_ShowIP(x, y, A)����ʾ�����ʮ����IP��ַ��	ע����x��y��0��ʼ��		//
//													//																				//
//	�汾��	V1.0									//	������OLED��SDA=IO2����ESP-12Fģ��LED��IO2��8266��OLED����IICͨ��ʱLED����	//
//													//																				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLEDͷ�ļ�

//#include "at_custom.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ip_addr.h"			// ��"espconn.h"ʹ�á�
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
#include "sntp.h"				// SNTP
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro
//==================================================================================


// �궨��
//==================================================================================
#define		ProjectName			"IIC_OLED"				// �������궨��
//==================================================================================

// ȫ�ֱ���
//==================================================================================

//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// user_init��entry of user application, init user function here
//=====================================================================================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");



//--------------------------------
	OLED_Init();	// OLED��ʼ��



// OLED��ʾ�ַ���
//-------------------------------------------------------------------------------------------------------------------------
	OLED_ShowString(0,0,"Project=");		// ��(0,0)��ʼ��ʾ

	OLED_ShowString(64,0,"IIC_OLED");		// ��(64,0)��ʼ��ʾ		// ��Ϊ����Project=��һ��8���ַ���������ռ�á�64������
//-------------------------------------------------------------------------------------------------------------------------



// OLED��ʾ�����ʮ����_IP��ַ��
//-------------------------------------------------------------------------------------------------------------------------
	u8 IP_Address[4];
	   IP_Address[0] = 192;
	   IP_Address[1] = 168;
	   IP_Address[2] = 4;
	   IP_Address[3] = 1;
//	u8 IP_Address[4] = { 192,168,4,1 };		// ������ʽ��ʾ�����ʮ����_IP��ַ��


	OLED_ShowString(0,2,"IP:");				// ��(0,2)��ʼ��ʾ		// ��Ϊ��Project=IIC_OLED����������ռ���ˡ�2��ҳ(2*8����)

	OLED_ShowIP(24, 2, IP_Address);			// ��(24,2)��ʼ��ʾ		// ��Ϊ����IP:��һ��3���ַ���������ռ�á�24������
//-------------------------------------------------------------------------------------------------------------------------


	OLED_ShowString(0,4,"0123456789ABCDEFGHIJKLMN");	// ��(0,4)��ʼ��ʾ
														// ��Ϊ��Project=IIC_OLED����IP:192.168.4.1����������ռ���ˡ�4��ҳ(4*8����)
}
//=====================================================================================================================================


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
