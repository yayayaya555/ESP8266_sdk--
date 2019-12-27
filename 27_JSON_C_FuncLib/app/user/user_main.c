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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																//	{										//
// ���̣�	JSON_C_FuncLib										//		"Shanghai": {						//
//																//			"temp": "30��",					//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0				//			"humid": "30%RH"				//
//																//		},									//
// ���ܣ�	�٣�����JSON����ʹ��os_sprintf,��ʽ���ַ�����		//		"Shenzhen": {						//
//																//			"temp": "35��",					//
//			�ڣ�����JSON����ʹ��C�������ṩ���ַ���������		//			"humid": 50						//
//																//		},									//
//	�汾��	V1.0												//		"result": "Shenzhen is too hot!"	//
//																//	}										//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ͷ�ļ�����
//==================================================================================
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����

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
//#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro

//==================================================================================


// �궨��
//==================================================================================
#define		ProjectName			"JSON_C_FuncLib"			// �������궨��

/* ��JSON_Tree��
//**************************************************************************
	{
    	"Shanghai":
    	{
        	"temp": "30��",
        	"humid": "30%RH"
    	},

    	"Shenzhen":
    	{
        	"temp": "35��",
        	"humid": 50
		},

    	"result": "Shenzhen is too hot!"
	}

//***************************************
*/

#define		JSON_Tree_Format	" { \n "								\
    								" \"Shanghai\": \n "				\
									" { \n "							\
        								" \"temp\": \"%s\", \n "		\
										" \"humid\": \"%s\" \n "		\
    								" }, \n "							\
																		\
									" \"Shenzhen\": \n "				\
									" { \n "							\
    									" \"temp\": \"%s\", \n "		\
										" \"humid\": %s \n "			\
    								" }, \n "							\
																		\
									" \"result\": \"%s\" \n "			\
								" } \n "

//**************************************************************************

//==================================================================================


// ȫ�ֱ���
//==================================================================================
char A_JSON_Tree[256] = {0};	// ���JSON��
//==================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ����JSON��
//===================================================================================================
void ICACHE_FLASH_ATTR Setup_JSON_Tree_JX(void)
{

	// ��ֵJSON������ֵJSON_Tree_Format�ַ����еĸ�ʽ�ַ���
	//--------------------------------------------------------------------------------------------
	os_sprintf(A_JSON_Tree, JSON_Tree_Format, "30��","30%RH","35��","50","Shenzhen is too hot!");

	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

	os_printf("%s",A_JSON_Tree);	// ���ڴ�ӡJSON��

	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");
}
//===================================================================================================


// ����JSON��
//===================================================================================================
int ICACHE_FLASH_ATTR Parse_JSON_Tree_JX(void)
{
	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

	char A_JSONTree_Value[64] = {0};	// JSON���ݻ�������

	char * T_Pointer_Head = NULL;		// ��ʱָ��
	char * T_Pointer_end = NULL;		// ��ʱָ��

	u8 T_Value_Len = 0;					// ��"ֵ"���ĳ���


	// ��"Shanghai"��
	//������������������������������������������������������������������������������������
	T_Pointer_Head = os_strstr(A_JSON_Tree, "\"Shanghai\"");		// ��"Shanghai"��
	os_printf("Shanghai:\n");

//	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��
//	T_Pointer_Head = os_strstr(T_Pointer_Head, "{");				// ��{��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"temp\"");			// ��"temp"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"") + 1;			// ��ֵ����ָ�롿
	T_Pointer_end = os_strstr(T_Pointer_Head, "\"");				// ��"ֵ"β��"��
	T_Value_Len = T_Pointer_end - T_Pointer_Head;					// ���㡾ֵ���ĳ���
	os_memcpy(A_JSONTree_Value, T_Pointer_Head, T_Value_Len);		// ��ȡ��ֵ��
	A_JSONTree_Value[T_Value_Len] = '\0';							// ��ֵ����'\0'��
	os_printf("\t temp:%s\n",A_JSONTree_Value);


//	T_Pointer_Head = os_strstr(T_Pointer_Head, ",");				// ��,��
//	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"");				// ��\"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"humid\"");		// ��"humid"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"") + 1;			// ��ֵ����ָ�롿
	T_Pointer_end = os_strstr(T_Pointer_Head, "\"");				// ��"ֵ"β��"��
	T_Value_Len = T_Pointer_end - T_Pointer_Head;					// ���㡾ֵ���ĳ���
	os_memcpy(A_JSONTree_Value, T_Pointer_Head, T_Value_Len);		// ��ȡ��ֵ��
	A_JSONTree_Value[T_Value_Len] = '\0';							// ��ֵ����'\0'��
	os_printf("\t humid:%s\n",A_JSONTree_Value);

	//	T_Pointer_Head = os_strstr(T_Pointer_Head, "}");			// ��}��
	//	T_Pointer_Head = os_strstr(T_Pointer_Head, ",");			// ��,��
	//������������������������������������������������������������������������������������


	// ��"Shenzhen"��
	//������������������������������������������������������������������������������������
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"Shenzhen\"");		// ��"Shenzhen"��
	os_printf("Shenzhen:\n");

//	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��
//	T_Pointer_Head = os_strstr(T_Pointer_Head, "{");				// ��{��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"temp\"");			// ��"temp"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"") + 1;			// ��ֵ����ָ�롿
	T_Pointer_end = os_strstr(T_Pointer_Head, "\"");				// ��"ֵ"β��"��
	T_Value_Len = T_Pointer_end - T_Pointer_Head;					// ���㡾ֵ���ĳ���
	os_memcpy(A_JSONTree_Value, T_Pointer_Head, T_Value_Len);		// ��ȡ��ֵ��
	A_JSONTree_Value[T_Value_Len] = '\0';							// ��ֵ����'\0'��
	os_printf("\t temp:%s\n",A_JSONTree_Value);


	//��ע��"humid"������Ӧ��ֵ�����֡�����ͬ������ASSIC���ʾ������û��""��
	//����������������������������������������������������������������������������
//	T_Pointer_Head = os_strstr(T_Pointer_Head, ",");				// ��,��
//	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"");				// ��\"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"humid\"");		// ��"humid"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��

	// ��ȡ���ֵ���ָ�롾����Ϊʮ������ʽ������û��""��
	//-----------------------------------------------------
	while(*T_Pointer_Head < '0' || *T_Pointer_Head > '9')	// �ų����ڡ�0��9����Χ�ڵ��ַ�
		T_Pointer_Head ++ ;

	T_Pointer_end = T_Pointer_Head;	// ��������βָ���ֵ

	// ��ȡ���ֵ�βָ��+1������Ϊʮ������ʽ������û��""��
	//-----------------------------------------------------
	while(*T_Pointer_end >= '0' && *T_Pointer_end <= '9')	// �����ڡ�0��9����Χ�ڵ��ַ�
		T_Pointer_end ++ ;

	T_Value_Len = T_Pointer_end - T_Pointer_Head;					// ���㡾ֵ(����)���ĳ���
	os_memcpy(A_JSONTree_Value, T_Pointer_Head, T_Value_Len);		// ��ȡ��ֵ(����)��
	A_JSONTree_Value[T_Value_Len] = '\0';							// ��ֵ����'\0'��
	os_printf("\t humid:%s\n",A_JSONTree_Value);

//	T_Pointer_Head = os_strstr(T_Pointer_Head, "}");				// ��}��
//	T_Pointer_Head = os_strstr(T_Pointer_Head, ",");				// ��,��
	//������������������������������������������������������������������������������������

	// ��"result"��
	//������������������������������������������������������������������������������������
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"result\"");		// ��"result"��
	T_Pointer_Head = os_strstr(T_Pointer_Head, ":");				// ��:��
	T_Pointer_Head = os_strstr(T_Pointer_Head, "\"") + 1;			// ��ֵ����ָ�롿
	T_Pointer_end = os_strstr(T_Pointer_Head, "\"");				// ��"ֵ"β��"��
	T_Value_Len = T_Pointer_end - T_Pointer_Head;					// ���㡾ֵ���ĳ���
	os_memcpy(A_JSONTree_Value, T_Pointer_Head, T_Value_Len);		// ��ȡ��ֵ��
	A_JSONTree_Value[T_Value_Len] = '\0';							// ��ֵ����'\0'��
	os_printf("result:%s\n",A_JSONTree_Value);

	T_Pointer_Head = os_strstr(T_Pointer_Head, "}");			// ��}��
	//������������������������������������������������������������������������������������

    os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

    return 0 ;
}
//===================================================================================================



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

	os_printf("JSON_Tree_Format:\n%s", JSON_Tree_Format);	// ��ӡJSON����ʽ

	Setup_JSON_Tree_JX();		// ����JSON��

	Parse_JSON_Tree_JX();		// ����JSON��
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
