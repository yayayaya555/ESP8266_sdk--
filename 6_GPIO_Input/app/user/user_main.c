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

//////////////////////////////////////////////////////
//													//
// 工程：	GPIO_Input								//
//													//
// 平台：	【技新电子】物联网开发板 ESP8266 V1.0	//
//													//
// 功能：	①：初始化LED(GPIO4)					//
//													//
//			②：初始化按键：GPIO0设为输入(外部上拉)	//
//													//
//			③：按键按下时LED亮，LED松开时LED灭		//
//													//
// 版本：	V1.0									//
//													//
//////////////////////////////////////////////////////


// 头文件引用
//==================================================================================
#include "user_config.h"		// 用户配置
#include "driver/uart.h"  		// 串口

//#include "at_custom.h"
#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
//#include "espconn.h"
//#include "espnow.h"
#include "ets_sys.h"			// 回调函数
//#include "gpio.h"
//#include "ip_addr.h"
//#include "mem.h"
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX、软件定时器
//#include "ping.h"
//#include "pwm.h"
//#include "queue.h"
//#include "smartconfig.h"
//#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro
//==================================================================================


// 宏定义
//==================================================================================
#define		ProjectName		"GPIO_Input"		// 工程名宏定义
//==================================================================================


// 毫秒延时函数
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// user_init：entry of user application, init user function here
//=================================================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// 初始化LED(注意【PIN_NAME】、【FUNC】、【gpio_no】不要混淆)
	//-------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO_4设为IO口
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// GPIO_4 = 1

	// 初始化按键(BOOT == GPIO0)
	//----------------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,	FUNC_GPIO0);	// GPIO_0设为IO口
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));						// GPIO_0失能输出(默认)
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0失能内部上拉(默认)
//	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0使能内部上拉


	while(1)
	{
		system_soft_wdt_feed();								// 喂狗，防止复位

		if( GPIO_INPUT_GET(GPIO_ID_PIN(0)) == 0 )			// 读取GPIO_0电平
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);				// LED亮
		else
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);				// LED灭
	}
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

