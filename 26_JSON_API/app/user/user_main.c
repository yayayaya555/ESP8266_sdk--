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


//////////////////////////////////////////////////////////////////////////////////////////////////////
//														//	{										//
// ���̣�	JSON_API									//		"Shanghai": {						//
//														//			"temp": "30��",					//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0		//			"humid": "30%RH"				//
//														//		},									//
// ���ܣ�	�٣�����JSON����ʹ��JSON_API��				//		"Shenzhen": {						//
//														//			"temp": "35��",					//
//			�ڣ�����JSON����ʹ��JSON_API��				//			"humid": 50						//
//														//		},									//
//	�汾��	V1.0										//		"result": "Shenzhen is too hot!"	//
//														//	}										//
//////////////////////////////////////////////////////////////////////////////////////////////////////


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

#include "user_json.h"			// JSON
//==================================================================================


/* ��JSON_Tree��
//===========================================
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

//===========================================
*/


// �궨��
//==================================================================================
#define		ProjectName			"JSON_API"			// �������궨��
//==================================================================================


// ȫ�ֱ���
//==================================================================================
char A_JSON_Tree[256] = {0};	// ���JSON��
//==================================================================================

// ��������
//============================================================================================================
int ICACHE_FLASH_ATTR JSON_Tree_Parse_Cb(struct jsontree_context *js_ctx, struct jsonparse_state *parser);
//============================================================================================================


// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ����JSON���Ļص�������JSON����ֵ
//��jsontree_path_name(...)��		��ȡJSON����Ӧ��ȵġ�"��"����ָ�롿
//��jsontree_write_string(...)��	��ֵJSON����Ӧ��ȵġ�"��"��(���ַ�����ʽ��ֵ)��
//��jsontree_write_atom(...)��		��ֵJSON����Ӧ��ȵġ�"��"��(����ֵ����ʽ��ֵ)��
//=======================================================================================================================
LOCAL int ICACHE_FLASH_ATTR JSON_AssignValue_Cb(struct jsontree_context *js_ctx)	// �٣���������JSON���ĺ���
{
	const char * P_Key_current = jsontree_path_name(js_ctx,js_ctx->depth-1);	// ��ȡָ��ǰ��"��"����ָ��

	if( os_strncmp(P_Key_current, "result", os_strlen("result")) == 0 )			// �жϵ�ǰ��"��"�� ?= "result"
	{
		jsontree_write_string(js_ctx,"Shenzhen is too hot!");	// ����"ֵ"="Shenzhen is too hot!"��д���Ӧλ��
	}

	else	// ��ǰ��"��"�� != "result"���ж���һ��ָ��
	{
		const char * P_Key_upper = jsontree_path_name(js_ctx,js_ctx->depth-2);		// ��ȡָ����һ�㡾"��"����ָ��


		if(os_strncmp(P_Key_upper, "Shanghai", os_strlen("Shanghai")) == 0)			// �ж���һ�㡾"��"�� ?= "Shanghai"
		{
			if(os_strncmp(P_Key_current, "temp", os_strlen("temp")) == 0)			// �жϵ�ǰ��"��"�� ?= "temp"
			{
				jsontree_write_string(js_ctx,"30��");	// ����"ֵ"="30��"��д���Ӧλ��
			}


			if(os_strncmp(P_Key_current, "humid", os_strlen("humid")) == 0)			// �жϵ�ǰ��"��"�� ?= "humid"
			{
				jsontree_write_string(js_ctx,"30%RH");	// ����"ֵ"="30%RH"��д���Ӧλ��
			}

		}

		else if( os_strncmp(P_Key_upper, "Shenzhen", os_strlen("Shenzhen")) == 0 )	// �ж���һ�㡾"��"�� ?= "Shenzhen"
		{
			if(os_strncmp(P_Key_current, "temp", os_strlen("temp")) == 0)			// �жϵ�ǰ��"��"�� ?= "temp"
			{
				jsontree_write_string(js_ctx,"35��");	// ����"ֵ"="35��"��д���Ӧλ��
			}


			if(os_strncmp(P_Key_current, "humid", os_strlen("humid")) == 0)			// �жϵ�ǰ��"��"�� ?= "humid"
			{
				jsontree_write_atom(js_ctx,"50");		// ����"ֵ" = 50��д���Ӧλ��
			}
		}
	}

    return 0;
}
//=======================================================================================================================



// JSON���������
//============================================================================================================================

// ���á�����JSON������������JSON�����Ļص�����
//��������÷���һ����"ֵ"�������鷳�����Զ�����JSON���ƺ�����
struct jsontree_callback JSON_Tree_Set = JSONTREE_CALLBACK(JSON_AssignValue_Cb, JSON_Tree_Parse_Cb);	// �ṹ�嶨��


//����������V_Key_1		��ֵ�ԣ�"temp":{ &JSON_Tree_Set }, "humid":{ &JSON_Tree_Set }��
//����������V_Key_2		��ֵ�ԣ�"temp":{ &JSON_Tree_Set }, "humid":{ &JSON_Tree_Set }��
//--------------------------------------------------------------------------------------------------------------------------
JSONTREE_OBJECT(V_Key_1,JSONTREE_PAIR("temp", &JSON_Tree_Set),JSONTREE_PAIR("humid", &JSON_Tree_Set));	// ���á�"��":"ֵ"��
JSONTREE_OBJECT(V_Key_2,JSONTREE_PAIR("temp", &JSON_Tree_Set),JSONTREE_PAIR("humid", &JSON_Tree_Set));	// ���á�"��":"ֵ"��


//����������V_JSON		��ֵ�ԣ�"Shanghai":{ &V_Key_1 }, "Shenzhen":{ &V_Key_1 }, "result":{ &JSON_Tree_Set }��
//----------------------------------------------------------------------------------------------------------------
JSONTREE_OBJECT(	V_JSON, 	JSONTREE_PAIR("Shanghai", &V_Key_1),	JSONTREE_PAIR("Shenzhen", &V_Key_2),	\
								JSONTREE_PAIR("result", &JSON_Tree_Set)	);		// ���á�"��":"ֵ"��


//---------------------------------------------------------------------------
// ע��JSONTREE_OBJECT( ����1, ����2, ����3, ... )

//������1�����ɵ�JSON����������ơ���������2����ֵ�ԡ���������3����ֵ�ԡ�...
//---------------------------------------------------------------------------
//����������Object_JOSN	  ��ֵ�ԣ�"K_JOSN" : { &V_JSON } ��

//��ע����һ����JSON��������"��"������ʾ��ֻ��ʾ��{ &V_JSON } ��
//----------------------------------------------------------------------------------
JSONTREE_OBJECT(Object_JOSN,JSONTREE_PAIR("K_JOSN", &V_JSON));		// ����JOSN����

//============================================================================================================================


// ����JSON��
//================================================================================
void ICACHE_FLASH_ATTR Setup_JSON_Tree_JX(void)
{
	//������JSON����
	//������1����JSON�����ָ�� / ����2����JSON����ļ� / ����3��JSON������ָ�롿
	//---------------------------------------------------------------------------
	json_ws_send((struct jsontree_value *)&Object_JOSN, "K_JOSN", A_JSON_Tree);


	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

	os_printf("%s",A_JSON_Tree);	// ���ڴ�ӡJSON��

	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");
}
//================================================================================


// ����JSON��
//=====================================================================================================
void ICACHE_FLASH_ATTR Parse_JSON_Tree_JX(void)
{
	struct jsontree_context js;		// JSON�������ṹ��

	// ��Ҫ������JSON��������ϵ
	//������1��JSON�������ṹ��ָ��		����2��JSON���ĵڶ���������ָ��		����3��json_putchar������
	//-----------------------------------------------------------------------------------------------
	jsontree_setup(&js, (struct jsontree_value *)&V_JSON, json_putchar);

	// ����JSON��
	//������1��JSON�������ṹ��ָ��		����2��Ҫ������JSON����ָ�롿
	json_parse(&js, A_JSON_Tree);	// ִ��������䣬������ö�Ӧ��JSON�������ص�����
}
//=====================================================================================================


// ����JSON���Ļص�����
//========================================================================================================================================
int ICACHE_FLASH_ATTR JSON_Tree_Parse_Cb(struct jsontree_context *js_ctx, struct jsonparse_state *parser)
{
	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

    int type;						// �ַ�����

    char buffer[64] = {0};			// ���桾ֵ��������

    type = jsonparse_next(parser);	// ��{����JSON����Ŀ�����ַ�
    os_printf("%c\n", type);

    type = jsonparse_next(parser);	// ����N������һ����������Shanghai��


    if(type != 0)	// �ж��Ƿ����ַ���
    {
        if (type == JSON_TYPE_PAIR_NAME) 	// �ж��Ƿ��ǡ���N��
        {
            if (jsonparse_strcmp_value(parser, "Shanghai") == 0)	// ��Shanghai��
            {
            	os_printf("\t Shanghai{2} \n");

            	type = jsonparse_next(parser);		// ��:��
            	type = jsonparse_next(parser);		// ��{��

            	type = jsonparse_next(parser);  	// ����N��
                if (jsonparse_strcmp_value(parser, "temp") == 0)	// ��temp��
                {
                	type = jsonparse_next(parser);		// ��:��
                	type = jsonparse_next(parser);  	// ��"��

                	if (type == JSON_TYPE_STRING)		// �ж��Ƿ��ǡ�"��
					{
						jsonparse_copy_value(parser, buffer, sizeof(buffer));	// ��ȡ��������Ӧ��ֵ
						os_printf("\t\t temp: %s\n", buffer);		// ��30�桿
					}

                	type = jsonparse_next(parser);  	// ��,��

                	type = jsonparse_next(parser);  	// ����N��
                	if (jsonparse_strcmp_value(parser, "humid") == 0)	// ��humid��
                	{
                		type = jsonparse_next(parser);  	// ��:��

                		type = jsonparse_next(parser);  	// ��"��
						if (type == JSON_TYPE_STRING)		// �ж��Ƿ��ǡ�"��
						{
							jsonparse_copy_value(parser, buffer, sizeof(buffer));	// ��ȡ��������Ӧ��ֵ
							os_printf("\t\t humid: %s\n", buffer);	// ��30%RH��


							type = jsonparse_next(parser);  	// ��}��
							type = jsonparse_next(parser);  	// ��,��


							type = jsonparse_next(parser);  	//����N�����ڶ�����������Shenzhen��
							if (jsonparse_strcmp_value(parser, "Shenzhen") == 0)	// ��Shenzhen��
							{
								os_printf("\t Shenzhen{2} \n");

								jsonparse_next(parser);		// ��:��
								jsonparse_next(parser);		// ��{��

								jsonparse_next(parser);  	// ����N��
								if (jsonparse_strcmp_value(parser, "temp") == 0)	// ��temp��
								{
									type = jsonparse_next(parser);		// ��:��

									type = jsonparse_next(parser);  	// ��"��
									if (type == JSON_TYPE_STRING)		// �ж��Ƿ��ǡ�"��
									{
										jsonparse_copy_value(parser, buffer, sizeof(buffer));	// ��ȡ��������Ӧ��ֵ
										os_printf("\t\t temp: %s\n", buffer);	// ��35�桿

										type = jsonparse_next(parser);  	// ��,��

										type = jsonparse_next(parser);  	// ����N��
										if (jsonparse_strcmp_value(parser, "humid") == 0)	// ��humid��
										{
											type = jsonparse_next(parser);  	// ��:��

											type = jsonparse_next(parser);  	// ��0��("ֵ" = ��ֵ)
											if (type == JSON_TYPE_NUMBER)		// �ж��Ƿ��ǡ�0��(��ֵ == ASSIC����ʽ)
											{
												jsonparse_copy_value(parser, buffer, sizeof(buffer));	// ��ȡ��������Ӧ��ֵ
												os_printf("\t\t humid: %s\n", buffer);	// ��50%RH��

												type = jsonparse_next(parser);  	// ��}��
												type = jsonparse_next(parser);  	// ��,��

												type = jsonparse_next(parser);  	//����N������������������result��
												if (jsonparse_strcmp_value(parser, "result") == 0)		// ��result��
												{
													type = jsonparse_next(parser);  // ��:��

													type = jsonparse_next(parser);  // ��"��
													if (type == JSON_TYPE_STRING)	// �ж��Ƿ��ǡ�"��
													{
														jsonparse_copy_value(parser, buffer, sizeof(buffer));	// ��ȡ��������Ӧ��ֵ
														os_printf("\t result: %s\n", buffer);	//��Shenzhen is too hot!��

														type = jsonparse_next(parser);	// ��}����JSON����Ŀ��β�ַ�
														os_printf("%c\n", type);
													}
												}
											}
										}
									}
								}
							}
						}
                	}
                }
            }
        }
    }

    os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

    return 0;
}
//========================================================================================================================================



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
