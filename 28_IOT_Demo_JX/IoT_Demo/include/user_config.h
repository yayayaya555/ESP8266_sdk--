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

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define ESP_PLATFORM		1	// ʹ��ESPƽ̨(����)
#define LEWEI_PLATFORM		0

#define USE_OPTIMIZE_PRINTF

//������ƽ̨�µ��豸ѡ��
//----------------------------------------
#if ESP_PLATFORM
#define PLUG_DEVICE			0	// ����
#define LIGHT_DEVICE		1	// �ƹ�
#define SENSOR_DEVICE		0	// ������


// �������µ��豸ѡ��
//--------------------------------------------------------
#if SENSOR_DEVICE
#define HUMITURE_SUB_DEVICE         1	// ��ʪ�ȴ�����
#define FLAMMABLE_GAS_SUB_DEVICE    0	// ��ȼ���崫����
#endif


//#define SERVER_SSL_ENABLE		// ������SSLʹ��
//#define CLIENT_SSL_ENABLE		// �ͻ���SSLʹ��
//#define UPGRADE_SSL_ENABLE	// ����SSLʹ��


// DNS����
//-------------------------------------------------------------------------------------------------------
#define USE_DNS		// ʹ��DNS

#ifdef  USE_DNS

#define ESP_DOMAIN		"iot.espressif.cn"		// 115.29.202.58��iot.espressif.cn(ʱ�����й�����ʱ��)��
//#define ESP_DOMAIN	"iot.espressif.com"		// 119.9.91.208��iot.espressif.com(ʱ�����������α�׼ʱ��)��

#endif
//-----------------------------------------------------------------


// softAP��������
//-------------------------------------
//#define SOFTAP_ENCRYPT

#ifdef SOFTAP_ENCRYPT
#define PASSWORD	"v*%W>L<@i&Nxe!"
#endif


// ������˯������
//-------------------------------------------
#if SENSOR_DEVICE
#define SENSOR_DEEP_SLEEP

#if HUMITURE_SUB_DEVICE
#define SENSOR_DEEP_SLEEP_TIME    30000000
#elif FLAMMABLE_GAS_SUB_DEVICE
#define SENSOR_DEEP_SLEEP_TIME    60000000
#endif
#endif


// �ƹ�����
//--------------------------------------
#if LIGHT_DEVICE
#define USE_US_TIMER	// ΢�붨ʱ(us)
#endif

// �ƹ�/��������
//-------------------------------------
#if PLUG_DEVICE || LIGHT_DEVICE
#define BEACON_TIMEOUT  150000000
#define BEACON_TIME     50000
#endif

// AP����
//--------------------------------------------
#define AP_CACHE           1	// ʹ��AP����

#if AP_CACHE
#define AP_CACHE_NUMBER    5	// ��໺��5��
#endif

// �������������ƽ̨
//---------------------
#elif LEWEI_PLATFORM
#endif


#endif	/*__USER_CONFIG_H__*/

