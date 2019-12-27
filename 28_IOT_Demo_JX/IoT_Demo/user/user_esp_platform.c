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

#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"

#include "espconn.h"
#include "user_esp_platform.h"
#include "user_iot_version.h"
#include "upgrade.h"

#if ESP_PLATFORM

#define ESP_DEBUG

#ifdef ESP_DEBUG
#define ESP_DBG os_printf	//��ESP_DBG��==��os_printf��
#else
#define ESP_DBG
#endif



// �޸��豸���ݵ㡾��С����ӡ�
//-----------------------------------------------------------------------------------
//��"path"��ѡ���������ݵ��"����"��
//��"Authorization"��ѡ��������֤�豸���(Master Device Key)��
//��"datapoints"��ѡ������ά��(x��y��z��k��l)��
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define Modify_DATA_POINT	"{\"path\": \"/v1/datastreams/%s/datapoints/\", \"method\": \"POST\", \"meta\": {\"Authorization\": \"token %s\"}, \"body\": {\"datapoints\":[{\"x\":%d}]}}\n"


//���ƶ��豸������(������ִ��RESET����)
//-------------------------------------------------------------------------------------------------------------
//��"nonce"��ѡ���������� MD5-NONCE��SHA1-NONCE��HMAC-SHA1-NONCE �����㷨����һ��������������������������㷨��
//��"encrypt_method"��Ҫ���������ʽ(������ΪPLAIN��ʽ)��
//��"token"��ѡ��������40λ����ַ�������"ӵ������Ȩ"����ʵ������п����ǳ���˵��������ġ���ά�롱��
//��"bssid"��ѡ���������ø��豸��MAC��ַ��
//��"rom_version"��ѡ�������汾��Ϣ��
//��"Authorization"��ѡ��������֤�豸���(Master Device Key)��
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ACTIVE_FRAME    "{\"nonce\": %d,\"path\": \"/v1/device/activate/\", \"method\": \"POST\", \"body\": {\"encrypt_method\": \"PLAIN\", \"token\": \"%s\", \"bssid\": \""MACSTR"\",\"rom_version\":\"%s\"}, \"meta\": {\"Authorization\": \"token %s\"}}\n"

#if PLUG_DEVICE
#include "user_plug.h"

#define RESPONSE_FRAME  "{\"status\": 200, \"datapoint\": {\"x\": %d}, \"nonce\": %d, \"deliver_to_device\": true}\n"
#define FIRST_FRAME     "{\"nonce\": %d, \"path\": \"/v1/device/identify\", \"method\": \"GET\",\"meta\": {\"Authorization\": \"token %s\"}}\n"

#elif LIGHT_DEVICE
#include "user_light.h"


#define RESPONSE_FRAME  "{\"status\": 200,\"nonce\": %d, \"datapoint\": {\"x\": %d,\"y\": %d,\"z\": %d,\"k\": %d,\"l\": %d},\"deliver_to_device\":true}\n"

//�������豸������ƽ̨��ʶ��ݣ��������豸���������ӣ�ESP8266��������ƽ̨�������Ӻ�������ƽ̨��ʶ��ݣ���������֮��ʹ��һ�μ��ɣ�
//-------------------------------------------------------------------------------------------------------------------------------
//��"nonce"��ѡ���������� MD5-NONCE��SHA1-NONCE��HMAC-SHA1-NONCE �����㷨����һ��������������������������㷨��
//��"Authorization"��ѡ��������֤�豸���(Master Device Key)��
//-----------------------------------------------------------------------------------------------------------------------------------------------
#define FIRST_FRAME     "{\"nonce\": %d, \"path\": \"/v1/device/identify\", \"method\": \"GET\",\"meta\": {\"Authorization\": \"token %s\"}}\n"

#elif SENSOR_DEVICE
#include "user_sensor.h"

#if HUMITURE_SUB_DEVICE
#define UPLOAD_FRAME  "{\"nonce\": %d, \"path\": \"/v1/datastreams/tem_hum/datapoint/\", \"method\": \"POST\", \
\"body\": {\"datapoint\": {\"x\": %s%d.%02d,\"y\": %d.%02d}}, \"meta\": {\"Authorization\": \"token %s\"}}\n"
#elif FLAMMABLE_GAS_SUB_DEVICE
#define UPLOAD_FRAME  "{\"nonce\": %d, \"path\": \"/v1/datastreams/flammable_gas/datapoint/\", \"method\": \"POST\", \
\"body\": {\"datapoint\": {\"x\": %d.%03d}}, \"meta\": {\"Authorization\": \"token %s\"}}\n"
#endif

LOCAL uint32 count = 0;
#endif

#define UPGRADE_FRAME  "{\"path\": \"/v1/messages/\", \"method\": \"POST\", \"meta\": {\"Authorization\": \"token %s\"},\
\"get\":{\"action\":\"%s\"},\"body\":{\"pre_rom_version\":\"%s\",\"rom_version\":\"%s\"}}\n"



#if PLUG_DEVICE || LIGHT_DEVICE

//�������豸����ʱ�ͷ�����(������ƽ̨)��������
//------------------------------------------------------------
//��"Authorization"��ѡ��������֤�豸���(Master Device Key)��
//-----------------------------------------------------------------------------------------------------------------------
#define BEACON_FRAME    "{\"path\": \"/v1/ping/\", \"method\": \"POST\",\"meta\": {\"Authorization\": \"token %s\"}}\n"


//�������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)
//-------------------------------------------------
//��"nonce"��������ƽ̨Rpcָ���"nonce"����Ӧ��ֵ��
//--------------------------------------------------------------------------------------------
#define RPC_RESPONSE_FRAME  "{\"status\": 200, \"nonce\": %d, \"deliver_to_device\": true}\n"


// �������豸�������������ʱ��	��iot.espressif.cn(ʱ�����й�����ʱ��)��	��iot.espressif.com(ʱ�����������α�׼ʱ��)��
//-----------------------------------------------------------------------------------------------------------------------
//��"Authorization"��ѡ��������֤�豸���(Master Device Key)��
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TIMER_FRAME     "{\"body\": {}, \"get\":{\"is_humanize_format_simple\":\"true\"},\"meta\": {\"Authorization\": \"Token %s\"},\"path\": \"/v1/device/timers/\",\"post\":{},\"method\": \"GET\"}\n"

#define pheadbuffer "Connection: keep-alive\r\n\
Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Authorization: token %s\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"

LOCAL uint8 ping_status;		// ����״̬��1��ping success(�����ɹ�)	0����ƽ̨δӦ����������
LOCAL os_timer_t beacon_timer;	// ������ʱ�������ã������������Ƶ����ӣ�ÿ50�뷢��һ�����ݰ�(������)��
#endif

#ifdef USE_DNS
ip_addr_t esp_server_ip;	// ������IP��ַ�������ơ�iot.espressif.cn == 115.29.202.58	iot.espressif.com == 119.9.91.208
#endif

LOCAL struct espconn user_conn;		// �������ӽṹ��

LOCAL struct _esp_tcp user_tcp;		// TCP�ṹ��

LOCAL os_timer_t client_timer;		// Client��ʱ��(���������ʱ��֮һ)

struct esp_platform_saved_param esp_param;		//���ƶ�/���¡��豸�Ĳ���

LOCAL uint8 device_status;			//	�豸״̬��DEVICE_CONNECTING / DEVICE_ACTIVE_DONE / DEVICE_ACTIVE_FAIL / DEVICE_CONNECT_SERVER_FAIL

LOCAL uint8 device_recon_count = 0;		// ESP8266_TCP��������

LOCAL uint32 active_nonce = 0;			// ����������롿����ʱʹ�õ��㷨����������

LOCAL uint8 iot_version[20] = {0};		// ��Ʒ�汾

struct rst_info rtc_info;		// RTC��Ϣ�ṹ��

void user_esp_platform_check_ip(uint8 reset_flag);	// ��������

extern uint32 priv_param_start_sec;     // 0x7C(32MBit Flash)

/******************************************************************************
 * FunctionName : user_esp_platform_get_token
 * Description  : get the espressif's device token
 * Parameters   : token -- the parame point which write the flash
 * Returns      : none
*******************************************************************************/

// ��ȡ���ƶ��豸����esp_param.token
//====================================================================
void ICACHE_FLASH_ATTR user_esp_platform_get_token(uint8_t *token)
{
    if (token == NULL)
    {
        return;
    }

    // ����esp_param.token
    //----------------------------------------------------------
    os_memcpy(token, esp_param.token, sizeof(esp_param.token));
}
//====================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_set_token
 * Description  : save the token for the espressif's device
 * Parameters   : token -- the parame point which write the flash
 * Returns      : none
*******************************************************************************/

// ����esp_param.token�����ƶ��豸��Ϊδ����״̬����JSON����ʱ���յ�"token"���á�
//========================================================================================
void ICACHE_FLASH_ATTR user_esp_platform_set_token(uint8_t *token)
{
    if (token == NULL)	// �ж�����tokenָ��
    {
        return;
    }

    // esp_param.activeflag ==��0�����ƶ��豸δ����
    // esp_param.activeflag ==��1�����ƶ��豸�Ѽ���
    esp_param.activeflag = 0;

    os_memcpy(esp_param.token, token, os_strlen(token));	// ����esp_param.token

    // ����esp_param��Flash
    //-----------------------------------------------------------------------------------
    system_param_save_with_protect(priv_param_start_sec+1,&esp_param,sizeof(esp_param));
}
//========================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_set_active
 * Description  : set active flag
 * Parameters   : activeflag -- 0 or 1
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_esp_platform_set_active(uint8 activeflag)
{
    esp_param.activeflag = activeflag;	// ����esp_param.activeflag

    system_param_save_with_protect(priv_param_start_sec+1,&esp_param,sizeof(esp_param));	// ����esp_param.token
}

void ICACHE_FLASH_ATTR
user_esp_platform_set_connect_status(uint8 status)
{
    device_status = status;
}

/******************************************************************************
 * FunctionName : user_esp_platform_get_connect_status
 * Description  : get each connection step's status
 * Parameters   : none
 * Returns      : status
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR
user_esp_platform_get_connect_status(void)
{
    uint8 status = wifi_station_get_connect_status();

    if (status == STATION_GOT_IP) {
        status = (device_status == 0) ? DEVICE_CONNECTING : device_status;
    }

    ESP_DBG("status %d\n", status);
    return status;
}

/******************************************************************************
 * FunctionName : user_esp_platform_parse_nonce
 * Description  : parse the device nonce
 * Parameters   : pbuffer -- the recivce data point
 * Returns      : the nonce
*******************************************************************************/

// ��ȡ��"nonce"������Ӧ�ġ�"ֵ"��
//==================================================================================
int ICACHE_FLASH_ATTR user_esp_platform_parse_nonce(char *pbuffer)
{
    char *pstr = NULL;
    char *pparse = NULL;
    char noncestr[11] = {0};
    int nonce = 0;
    pstr = (char *)os_strstr(pbuffer, "\"nonce\": ");	// �ҵ���"nonce": ���׵�ַ

    if (pstr != NULL)
    {
        pstr += 9;
        pparse = (char *)os_strstr(pstr, ",");	// �ҵ���,���׵�ַ

        if (pparse != NULL)
        {
            os_memcpy(noncestr, pstr, pparse-pstr);	// ��ȡ��"nonce": ����Ӧ�ġ�"ֵ"��
        }

        else	// pparse == NULL
        {
            pparse = (char *)os_strstr(pstr, "}");

            if (pparse != NULL) {
                os_memcpy(noncestr, pstr, pparse - pstr);
            } else {
                pparse = (char *)os_strstr(pstr, "]");

                if (pparse != NULL) {
                    os_memcpy(noncestr, pstr, pparse - pstr);
                } else {
                    return 0;
                }
            }
        }


        nonce = atoi(noncestr);		// ����"ֵ"(�ַ���)��ת��Ϊ��������
    }

    return nonce;
}
//==================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_get_info
 * Description  : get and update the espressif's device status
 * Parameters   : pespconn -- the espconn used to connect with host
 *                pbuffer -- prossing the data point
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_esp_platform_get_info(struct espconn *pconn, uint8 *pbuffer)
{
    char *pbuf = NULL;
    int nonce = 0;

    pbuf = (char *)os_zalloc(packet_size);	// �����ڴ�

    nonce = user_esp_platform_parse_nonce(pbuffer);	// ��ȡ��"nonce"������Ӧ�ġ�"ֵ"��

    if (pbuf != NULL)
    {
#if PLUG_DEVICE
        os_sprintf(pbuf, RESPONSE_FRAME, user_plug_get_status(), nonce);
#elif LIGHT_DEVICE
        uint32 white_val;
        white_val = (PWM_CHANNEL>LIGHT_COLD_WHITE?user_light_get_duty(LIGHT_COLD_WHITE):0);
        os_sprintf(pbuf, RESPONSE_FRAME, nonce, user_light_get_period(),
                   user_light_get_duty(LIGHT_RED), user_light_get_duty(LIGHT_GREEN),
                   user_light_get_duty(LIGHT_BLUE),white_val );//50);
#endif

        ESP_DBG("%s\n", pbuf);
#ifdef CLIENT_SSL_ENABLE
        espconn_secure_sent(pconn, pbuf, os_strlen(pbuf));
#else
        espconn_sent(pconn, pbuf, os_strlen(pbuf));
#endif
        os_free(pbuf);
        pbuf = NULL;
    }
}

/******************************************************************************
 * FunctionName : user_esp_platform_set_info
 * Description  : prossing the data and controling the espressif's device
 * Parameters   : pespconn -- the espconn used to connect with host
 *                pbuffer -- prossing the data point
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_esp_platform_set_info(struct espconn *pconn, uint8 *pbuffer)
{
#if PLUG_DEVICE
    char *pstr = NULL;
    pstr = (char *)os_strstr(pbuffer, "plug-status");

    if (pstr != NULL) {
        pstr = (char *)os_strstr(pbuffer, "body");

        if (pstr != NULL) {

            if (os_strncmp(pstr + 27, "1", 1) == 0) {
                user_plug_set_status(0x01);
            } else if (os_strncmp(pstr + 27, "0", 1) == 0) {
                user_plug_set_status(0x00);
            }
        }
    }

#elif LIGHT_DEVICE
    char *pstr = NULL;
    char *pdata = NULL;
    char *pbuf = NULL;
    char recvbuf[10];
    uint16 length = 0;
    uint32 data = 0;
    static uint32 rr,gg,bb,cw,ww,period;
    ww=0;
    cw=0;
    extern uint8 light_sleep_flg;
    pstr = (char *)os_strstr(pbuffer, "\"path\": \"/v1/datastreams/light/datapoint/\"");

    if (pstr != NULL) {
        pstr = (char *)os_strstr(pbuffer, "{\"datapoint\": ");

        if (pstr != NULL) {
            pbuf = (char *)os_strstr(pbuffer, "}}");
            length = pbuf - pstr;
            length += 2;
            pdata = (char *)os_zalloc(length + 1);
            os_memcpy(pdata, pstr, length);

            pstr = (char *)os_strchr(pdata, 'x');

            if (pstr != NULL) {
                pstr += 4;
                pbuf = (char *)os_strchr(pstr, ',');

                if (pbuf != NULL) {
                    length = pbuf - pstr;
                    os_memset(recvbuf, 0, 10);
                    os_memcpy(recvbuf, pstr, length);
                    data = atoi(recvbuf);
                    period = data;
                    //user_light_set_period(data);
                }
            }

            pstr = (char *)os_strchr(pdata, 'y');

            if (pstr != NULL) {
                pstr += 4;
                pbuf = (char *)os_strchr(pstr, ',');

                if (pbuf != NULL) {
                    length = pbuf - pstr;
                    os_memset(recvbuf, 0, 10);
                    os_memcpy(recvbuf, pstr, length);
                    data = atoi(recvbuf);
                    rr=data;
                    os_printf("r: %d\r\n",rr);
                    //user_light_set_duty(data, 0);
                }
            }

            pstr = (char *)os_strchr(pdata, 'z');

            if (pstr != NULL) {
                pstr += 4;
                pbuf = (char *)os_strchr(pstr, ',');

                if (pbuf != NULL) {
                    length = pbuf - pstr;
                    os_memset(recvbuf, 0, 10);
                    os_memcpy(recvbuf, pstr, length);
                    data = atoi(recvbuf);
                    gg=data;
                    os_printf("g: %d\r\n",gg);
                    //user_light_set_duty(data, 1);
                }
            }

            pstr = (char *)os_strchr(pdata, 'k');

            if (pstr != NULL) {
                pstr += 4;;
                pbuf = (char *)os_strchr(pstr, ',');

                if (pbuf != NULL) {
                    length = pbuf - pstr;
                    os_memset(recvbuf, 0, 10);
                    os_memcpy(recvbuf, pstr, length);
                    data = atoi(recvbuf);
                    bb=data;
                    os_printf("b: %d\r\n",bb);
                    //user_light_set_duty(data, 2);
                }
            }

            pstr = (char *)os_strchr(pdata, 'l');

            if (pstr != NULL) {
                pstr += 4;;
                pbuf = (char *)os_strchr(pstr, ',');

                if (pbuf != NULL) {
                    length = pbuf - pstr;
                    os_memset(recvbuf, 0, 10);
                    os_memcpy(recvbuf, pstr, length);
                    data = atoi(recvbuf);
                    cw=data;
		      ww=data;
                    os_printf("cw: %d\r\n",cw);
		      os_printf("ww:%d\r\n",ww);   //chg
                    //user_light_set_duty(data, 2);
                }
            }

            os_free(pdata);
        }
    }
    
    if((rr|gg|bb|cw|ww) == 0){
        if(light_sleep_flg==0){

        }
        
    }else{
        if(light_sleep_flg==1){
            os_printf("modem sleep en\r\n");
            wifi_set_sleep_type(MODEM_SLEEP_T);
            light_sleep_flg =0;
        }
    }

    light_set_aim(rr,gg,bb,cw,ww,period);
    //user_light_restart();

#endif

    user_esp_platform_get_info(pconn, pbuffer);
}

/******************************************************************************
 * FunctionName : user_esp_platform_reconnect
 * Description  : reconnect with host after get ip
 * Parameters   : pespconn -- the espconn used to reconnect with host
 * Returns      : none
*******************************************************************************/

// TCP���������IP��DNS������TCP�������á�
//====================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_reconnect(struct espconn *pespconn)
{
    ESP_DBG("user_esp_platform_reconnect\n");

    user_esp_platform_check_ip(0);	// ��ѯESP8266��IP��ȡ���
}
//====================================================================================

/******************************************************************************
 * FunctionName : user_esp_platform_discon_cb
 * Description  : disconnect successfully with the host
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/

// TCP���ӶϿ��ɹ�������TCP������
//=================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_discon_cb(void *arg)
{
    struct espconn *pespconn = arg;
    struct ip_info ipconfig;
	struct dhcp_client_info dhcp_info;
    ESP_DBG("user_esp_platform_discon_cb\n");

#if (PLUG_DEVICE || LIGHT_DEVICE)
    os_timer_disarm(&beacon_timer);		// �ر�������ʱ��
#endif

    if (pespconn == NULL)	// ����������
    {
        return;		// ����
    }

    pespconn->proto.tcp->local_port = espconn_port();	// ��ȡ���ض˿�

#if (PLUG_DEVICE || SENSOR_DEVICE)
    user_link_led_output(1);
#endif

#if SENSOR_DEVICE
#ifdef SENSOR_DEEP_SLEEP

    if (wifi_get_opmode() == STATION_MODE) {
    	/***add by tzx for saving ip_info to avoid dhcp_client start****/
    	wifi_get_ip_info(STATION_IF, &ipconfig);

    	dhcp_info.ip_addr = ipconfig.ip;
    	dhcp_info.netmask = ipconfig.netmask;
    	dhcp_info.gw = ipconfig.gw ;
    	dhcp_info.flag = 0x01;
    	os_printf("dhcp_info.ip_addr = %d\n",dhcp_info.ip_addr);
    	system_rtc_mem_write(64,&dhcp_info,sizeof(struct dhcp_client_info));
        user_sensor_deep_sleep_enter();
    } else {
        os_timer_disarm(&client_timer);
        os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_reconnect, pespconn);
        os_timer_arm(&client_timer, SENSOR_DEEP_SLEEP_TIME / 1000, 0);
    }

#else
    os_timer_disarm(&client_timer);
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_reconnect, pespconn);
    os_timer_arm(&client_timer, 1000, 0);
#endif
#else
    user_esp_platform_reconnect(pespconn);		// TCP����
#endif
}
//=================================================================================================

/******************************************************************************
 * FunctionName : user_esp_platform_discon
 * Description  : A new incoming connection has been disconnected.
 * Parameters   : espconn -- the espconn used to disconnect with host
 * Returns      : none
*******************************************************************************/

// �Ͽ�TCP����
//=================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_discon(struct espconn *pespconn)
{
    ESP_DBG("user_esp_platform_discon\n");

#if (PLUG_DEVICE || SENSOR_DEVICE)
    user_link_led_output(1);
#endif

#ifdef CLIENT_SSL_ENABLE
    espconn_secure_disconnect(pespconn);
#else
    espconn_disconnect(pespconn);
#endif
}
//=================================================================================

/******************************************************************************
 * FunctionName : user_esp_platform_sent_cb
 * Description  : Data has been sent successfully and acknowledged by the remote host.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/

// �������ݷ��ͳɹ�_�ص�����
//====================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_sent_cb(void *arg)
{
    struct espconn *pespconn = arg;

    ESP_DBG("user_esp_platform_sent_cb\n");
}
//====================================================================

/******************************************************************************
 * FunctionName : user_esp_platform_sent
 * Description  : Processing the application data and sending it to the host
 * Parameters   : pespconn -- the espconn used to connetion with the host
 * Returns      : none
*******************************************************************************/

// TCP���ӽ�����ESP8266��������ƽ̨����TCP����(�����ʶ)
//=================================================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_sent(struct espconn *pespconn)
{
    uint8 devkey[token_size] = {0};					// ��Device_Key���豸��Կ
    uint32 nonce;
    char *pbuf = (char *)os_zalloc(packet_size);	// �������ݰ�(2*1024Byte)�ռ�

    os_memcpy(devkey, esp_param.devkey, 40);		// ��ȡesp_param.devkeyֵ


	// esp_param.activeflag ==��0�����ƶ��豸δ�����ʼֵ==0xFF��
	// esp_param.activeflag ==��1�����ƶ��豸�Ѽ���
	//------------------------------------------------------------------------------------------
    if(esp_param.activeflag == 0xFF)	// ��� == ��ʼֵ == 0xFF
    { esp_param.activeflag = 0; }		// ��0


    // ���뵽�����ݰ��ռ�(2*1024)
    //--------------------------------------------------------------------------
    if (pbuf != NULL)
    {
    	//���ƶ��豸��δ����
    	//----------------------------------------------------------
        if (esp_param.activeflag == 0)	// ��0�����ƶ��豸δ����
        {
            uint8 token[token_size] = {0};
            uint8 bssid[6];
            active_nonce = os_random()&0x7FFFFFFF;	// ��һ�μ����ȡ����ġ���������롿

            os_memcpy(token, esp_param.token, 40);	// ��ȡesp_param.tokenֵ

            wifi_get_macaddr(STATION_IF, bssid);	// ��ȡSTA��MAC��ַ


            // ��ʽ����ACTIVE_FRAME���������ƶ��豸����������"ACTIVE_FRAME"���塿
            //--------------------------------------------------------------------------------------
            os_sprintf(pbuf, ACTIVE_FRAME, active_nonce, token, MAC2STR(bssid),iot_version, devkey);
        }

#if SENSOR_DEVICE
#if HUMITURE_SUB_DEVICE
        else {
#if 0
            uint16 tp, rh;
            uint8 data[4];

            if (user_mvh3004_read_th(data)) {
                rh = data[0] << 8 | data[1];
                tp = data[2] << 8 | data[3];
            }

#else
            uint16 tp, rh;
            uint8 *data;
            uint32 tp_t, rh_t;
            data = (uint8 *)user_mvh3004_get_poweron_th();

            rh = data[0] << 8 | data[1];
            tp = data[2] << 8 | data[3];
#endif
            tp_t = (tp >> 2) * 165 * 100 / (16384 - 1);
            rh_t = (rh & 0x3fff) * 100 * 100 / (16384 - 1);

            if (tp_t >= 4000) {
                os_sprintf(pbuf, UPLOAD_FRAME, count, "", tp_t / 100 - 40, tp_t % 100, rh_t / 100, rh_t % 100, devkey);
            } else {
                tp_t = 4000 - tp_t;
                os_sprintf(pbuf, UPLOAD_FRAME, count, "-", tp_t / 100, tp_t % 100, rh_t / 100, rh_t % 100, devkey);
            }
        }

#elif FLAMMABLE_GAS_SUB_DEVICE
        else {
            uint32 adc_value = system_adc_read();

            os_sprintf(pbuf, UPLOAD_FRAME, count, adc_value / 1024, adc_value * 1000 / 1024, devkey);
        }

#endif
#else
        //���ƶ��豸���Ѽ���
        //----------------------------------------------------------
        else
        {
            nonce = os_random() & 0x7FFFFFFF;	// ��ȡ����ġ������롿

            //�������豸�����������ʶ��ݣ���������֮��ʹ��һ�μ��ɣ�����������"FIRST_FRAME"���塿
            //-------------------------------------------------------------------------------------
            os_sprintf(pbuf, FIRST_FRAME, nonce , devkey);
        }

#endif
        ESP_DBG("%s\n", pbuf);	// ���ڴ�ӡ ��Ҫ���͸������Ʒ������ġ�JSON�ַ�����

#ifdef CLIENT_SSL_ENABLE
        espconn_secure_sent(pespconn, pbuf, os_strlen(pbuf));
#else
        espconn_sent(pespconn, pbuf, os_strlen(pbuf));	// �������ݰ�
#endif

        os_free(pbuf);	// �ͷ����ݰ��ռ�
    }
}
//=================================================================================================================================

#if PLUG_DEVICE || LIGHT_DEVICE
/******************************************************************************
 * FunctionName : user_esp_platform_sent_beacon
 * Description  : sent beacon frame for connection with the host is activate
 * Parameters   : pespconn -- the espconn used to connetion with the host
 * Returns      : none
*******************************************************************************/

// ����������_��ʱ_�ص�����
//==========================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_sent_beacon(struct espconn *pespconn)
{
    if (pespconn == NULL)
    {
        return;
    }

    if (pespconn->state == ESPCONN_CONNECT)
    {
        if (esp_param.activeflag == 0)	// ���ƶ��豸��δ����
        {
            ESP_DBG("please check device is activated.\n");
            user_esp_platform_sent(pespconn);	// ����ƶ��豸��
        }

        // ���ƶ��豸���Ѽ���
        //------------------------------------------------------------------
        else
        {
            uint8 devkey[token_size] = {0};

            os_memcpy(devkey, esp_param.devkey, 40);	//��Master Device Key���豸��Կ

            ESP_DBG("user_esp_platform_sent_beacon %u\n", system_get_time());

            if (ping_status == 0)	// ����״̬��1��ping success(�����ɹ�)	0����ƽ̨δӦ����������
            {
                ESP_DBG("user_esp_platform_sent_beacon sent fail!\n");

                user_esp_platform_discon(pespconn);		// �Ͽ�TCP����
            }

            else
            {
                char *pbuf = (char *)os_zalloc(packet_size);	// �������ݰ��ռ�

                if (pbuf != NULL)
                {
                    os_sprintf(pbuf, BEACON_FRAME, devkey);		// ����BEACON_FRAME��ʽ(�������豸����ʱ�ͷ�����(������ƽ̨)��������)

#ifdef CLIENT_SSL_ENABLE
                    espconn_secure_sent(pespconn, pbuf, os_strlen(pbuf));
#else
                    espconn_sent(pespconn, pbuf, os_strlen(pbuf));	// �������ݰ�

                    ESP_DBG("%s\n", pbuf);	//
#endif

                    ping_status = 0;	// ����״̬��0���ȴ������Ƶ�����Ӧ��

                    os_timer_arm(&beacon_timer, BEACON_TIME, 0);	// ������ʱ�� ���¼�ʱ

                    os_free(pbuf);		// �ͷ����ݰ��ڴ�
                }
            }
        }
    }

    else
    {
        ESP_DBG("user_esp_platform_sent_beacon sent fail!\n");

        user_esp_platform_discon(pespconn);		// ������ƶ��豸��δ�����Ͽ�TCP����
    }
}
//==========================================================================================================


/******************************************************************************
 * FunctionName : user_platform_rpc_set_rsp
 * Description  : response the message to server to show setting info is received
 * Parameters   : pespconn -- the espconn used to connetion with the host
 *                nonce -- mark the message received from server
 * Returns      : none
*******************************************************************************/

//�������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)
//=====================================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_platform_rpc_set_rsp(struct espconn *pespconn, int nonce)
{
    char *pbuf = (char *)os_zalloc(packet_size);

    if (pespconn == NULL) {
        return;
    }

    os_sprintf(pbuf, RPC_RESPONSE_FRAME, nonce);	//��ʽ���������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)��JSON�ַ���

    ESP_DBG("%s\n", pbuf);

#ifdef CLIENT_SSL_ENABLE
    espconn_secure_sent(pespconn, pbuf, os_strlen(pbuf));
#else
    espconn_sent(pespconn, pbuf, os_strlen(pbuf));	//�������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)
#endif
    os_free(pbuf);
}
//=====================================================================================================================

/******************************************************************************
 * FunctionName : user_platform_timer_get
 * Description  : get the timers from server
 * Parameters   : pespconn -- the espconn used to connetion with the host
 * Returns      : none
*******************************************************************************/

// �����������ʱ��
//======================================================================================
LOCAL void ICACHE_FLASH_ATTR user_platform_timer_get(struct espconn *pespconn)
{
    uint8 devkey[token_size] = {0};

    char *pbuf = (char *)os_zalloc(packet_size);	// �������ݰ��ռ�

    os_memcpy(devkey, esp_param.devkey, 40);	// ��ȡ�豸��Կ��Master Device Key��

    if (pespconn == NULL)
    {
        return;
    }

    os_sprintf(pbuf, TIMER_FRAME, devkey);	// ��ʽ���������������ʱ�䡿��JSON���ݰ�

    ESP_DBG("%s\n", pbuf);	// ���ڴ�ӡ���ݰ�

#ifdef CLIENT_SSL_ENABLE
    espconn_secure_sent(pespconn, pbuf, os_strlen(pbuf));
#else
    espconn_sent(pespconn, pbuf, os_strlen(pbuf));	// �����������ʱ��
#endif
    os_free(pbuf);	// �ͷ��ڴ�
}
//======================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_upgrade_cb
 * Description  : Processing the downloaded data from the server
 * Parameters   : pespconn -- the espconn used to connetion with the host
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
user_esp_platform_upgrade_rsp(void *arg)
{
    struct upgrade_server_info *server = arg;
    struct espconn *pespconn = server->pespconn;
    uint8 devkey[41] = {0};
    uint8 *pbuf = NULL;
    char *action = NULL;

    os_memcpy(devkey, esp_param.devkey, 40);
    pbuf = (char *)os_zalloc(packet_size);

    if (server->upgrade_flag == true) {
        ESP_DBG("user_esp_platform_upgarde_successfully\n");
        action = "device_upgrade_success";
        os_sprintf(pbuf, UPGRADE_FRAME, devkey, action, server->pre_version, server->upgrade_version);
        ESP_DBG("%s\n",pbuf);

#ifdef CLIENT_SSL_ENABLE
        espconn_secure_sent(pespconn, pbuf, os_strlen(pbuf));
#else
        espconn_sent(pespconn, pbuf, os_strlen(pbuf));
#endif

        if (pbuf != NULL) {
            os_free(pbuf);
            pbuf = NULL;
        }
    } else {
        ESP_DBG("user_esp_platform_upgrade_failed\n");
        action = "device_upgrade_failed";
        os_sprintf(pbuf, UPGRADE_FRAME, devkey, action,server->pre_version, server->upgrade_version);
        ESP_DBG("%s\n",pbuf);

#ifdef CLIENT_SSL_ENABLE
        espconn_secure_sent(pespconn, pbuf, os_strlen(pbuf));
#else
        espconn_sent(pespconn, pbuf, os_strlen(pbuf));
#endif

        if (pbuf != NULL) {
            os_free(pbuf);
            pbuf = NULL;
        }
    }

    os_free(server->url);
    server->url = NULL;
    os_free(server);
    server = NULL;
}

/******************************************************************************
 * FunctionName : user_esp_platform_upgrade_begin
 * Description  : Processing the received data from the server
 * Parameters   : pespconn -- the espconn used to connetion with the host
 *                server -- upgrade param
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
user_esp_platform_upgrade_begin(struct espconn *pespconn, struct upgrade_server_info *server)
{
    uint8 user_bin[9] = {0};
    uint8 devkey[41] = {0};

    server->pespconn = pespconn;

    os_memcpy(devkey, esp_param.devkey, 40);
    os_memcpy(server->ip, pespconn->proto.tcp->remote_ip, 4);

#ifdef UPGRADE_SSL_ENABLE
    server->port = 443;
#else
    server->port = 80;
#endif

    server->check_cb = user_esp_platform_upgrade_rsp;
    server->check_times = 120000;

    if (server->url == NULL) {
        server->url = (uint8 *)os_zalloc(512);
    }

    if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1) {
        os_memcpy(user_bin, "user2.bin", 10);
    } else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2) {
        os_memcpy(user_bin, "user1.bin", 10);
    }

    os_sprintf(server->url, "GET /v1/device/rom/?action=download_rom&version=%s&filename=%s HTTP/1.0\r\nHost: "IPSTR":%d\r\n"pheadbuffer"",
               server->upgrade_version, user_bin, IP2STR(server->ip),
               server->port, devkey);
    ESP_DBG("%s\n",server->url);

#ifdef UPGRADE_SSL_ENABLE

    if (system_upgrade_start_ssl(server) == false) {
#else

    if (system_upgrade_start(server) == false) {
#endif
        ESP_DBG("upgrade is already started\n");
    }
}

#endif		// #if PLUG_DEVICE || LIGHT_DEVICE


/******************************************************************************
 * FunctionName : user_esp_platform_recv_cb
 * Description  : Processing the received data from the server
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
*******************************************************************************/

// �������ݽ��ճɹ��Ļص�����
//============================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
    char *pstr = NULL;
    LOCAL char pbuffer[1024 * 2] = {0};		// ���ٿռ䣬������յ���������
    struct espconn *pespconn = arg;

    ESP_DBG("user_esp_platform_recv_cb:%s\n", pusrdata);	// ��ӡ���յ����ַ���
    ESP_DBG("\n-----------------------------------------------------------------------------------\n");

#if (PLUG_DEVICE || LIGHT_DEVICE)
    os_timer_disarm(&beacon_timer);		// �ر�������ʱ��
#endif


    // ���յ��������ݳ��� == 1460
    //-----------------------------------------------------------------------------------------
    if (length == 1460)		// ���ı��ĵ�������1024�ֽڣ����ܱ��ĵ�������1460�ֽ�
    {
        os_memcpy(pbuffer, pusrdata, length);	// �����յ������ݷ���pbuffer
    }

    else	// ���յ��������ݳ��� != 1460
    {
        struct espconn *pespconn = (struct espconn *)arg;

        os_memcpy(pbuffer+os_strlen(pbuffer), pusrdata, length);	// �����յ������ݷ���pbuffer[ƫ��0]


        // �жϽ��յ�����Ϣ�Ƿ������"activate_status": �������жϡ���������� active_nonce���Ƿ���ͬ
        //----------------------------------------------------------------------------------------------------------------------------------
        if ((pstr = (char *)os_strstr(pbuffer, "\"activate_status\": ")) != NULL && user_esp_platform_parse_nonce(pbuffer) == active_nonce)
        {
            if (os_strncmp(pstr+19,"1",1) == 0) 	// �жϼ���״ֵ̬?=1
            {
                ESP_DBG("device activates successful.\n");

                device_status = DEVICE_ACTIVE_DONE;	// �ƶ��豸����ɹ�

                esp_param.activeflag = 1;	// ���ƶ��豸������״̬ = 1

                system_param_save_with_protect(priv_param_start_sec+1, &esp_param, sizeof(esp_param));	// �����ƶ��豸������״̬��¼��������

                user_esp_platform_sent(pespconn);	// ����FIRST_FRAME

                if(LIGHT_DEVICE)
                {
                    system_restart();	// �����Light����λ
                }
            }

            else
            {
                ESP_DBG("device activates failed.\n");
                device_status = DEVICE_ACTIVE_FAIL;		// �ƶ��豸����ʧ��
            }
        }


#if (PLUG_DEVICE || LIGHT_DEVICE)

        // ���ǡ��ƶ��豸������Ļ�Ӧ��Ϣ
        //������������������������������������������������������������������������������������������������������������
        // ָ�"action": "sys_upgrade"��(֪ͨ�豸��Ҫ����)
        //-----------------------------------------------------------------------------------------------
        else if ((pstr = (char *)os_strstr(pbuffer, "\"action\": \"sys_upgrade\"")) != NULL)
        {
        	if ((pstr = (char *)os_strstr(pbuffer, "\"version\":")) != NULL) {
                struct upgrade_server_info *server = NULL;
                int nonce = user_esp_platform_parse_nonce(pbuffer);		// ��ȡ��"nonce"������Ӧ�ġ�"ֵ"��
                user_platform_rpc_set_rsp(pespconn, nonce);		//�������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)

                server = (struct upgrade_server_info *)os_zalloc(sizeof(struct upgrade_server_info));
                os_memcpy(server->upgrade_version, pstr + 12, 16);
                server->upgrade_version[15] = '\0';
                os_sprintf(server->pre_version,"%s%d.%d.%dt%d(%s)",VERSION_TYPE,IOT_VERSION_MAJOR,\
                    	IOT_VERSION_MINOR,IOT_VERSION_REVISION,device_type,UPGRADE_FALG);
                user_esp_platform_upgrade_begin(pespconn, server);
            }
        }

        // ָ�"action": "sys_reboot"��(֪ͨ�豸��Ҫ����)
        //-------------------------------------------------------------------------------------
        else if ((pstr = (char *)os_strstr(pbuffer, "\"action\": \"sys_reboot\"")) != NULL)
        {
            os_timer_disarm(&client_timer);
            os_timer_setfn(&client_timer, (os_timer_func_t *)system_upgrade_reboot, NULL);
            os_timer_arm(&client_timer, 1000, 0);
        }

        else if ((pstr = (char *)os_strstr(pbuffer, "/v1/device/timers/")) != NULL)
        {
            int nonce = user_esp_platform_parse_nonce(pbuffer);		// ��ȡ��"nonce"������Ӧ�ġ�"ֵ"��
            user_platform_rpc_set_rsp(pespconn, nonce);		//�������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)
            os_timer_disarm(&client_timer);
            os_timer_setfn(&client_timer, (os_timer_func_t *)user_platform_timer_get, pespconn);
            os_timer_arm(&client_timer, 2000, 0);
        }

        // ���յ���"method"����
        //��������������������������������������������������������������������������������������������������������
        else if ((pstr = (char *)os_strstr(pbuffer, "\"method\": ")) != NULL)
        {
            if (os_strncmp(pstr + 11, "GET", 3) == 0)	// �жϡ�"ֵ"��?= "GET"
            {
            	//####################################################################
            	//user_esp_platform_get_info(pespconn, pbuffer);	//����С��ע�͵���
            	//####################################################################

            	//����С����ӵġ�
            	//##################################################################################################################
            	int nonce = user_esp_platform_parse_nonce(pbuffer);		// ��ȡ��"nonce"������Ӧ�ġ�"ֵ"��
            	user_platform_rpc_set_rsp(pespconn, nonce);		//�������豸������ƽ̨Ӧ��(�ش���ƽ̨��Rpcָ��)

            	if( ((char *)os_strstr(pbuffer, "{\"deliver_to_device\": true, \"get\": {\"action\": \"LED_ON\"}")) != NULL )
            	{
            		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);
            		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);		// LED��
            	}
            	else if( ((char *)os_strstr(pbuffer, "{\"deliver_to_device\": true, \"get\": {\"action\": \"LED_OFF\"}")) != NULL )
            	{
            		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);
            		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);		// LED��
            	}
            	//##################################################################################################################

            }
            else if (os_strncmp(pstr + 11, "POST", 4) == 0)	// �жϡ�"ֵ"��?= "POST"
            {
            	//####################################################################
                //user_esp_platform_set_info(pespconn, pbuffer);	//����С��ע�͵���
                //####################################################################
            }
        }
        //��������������������������������������������������������������������������������������������������������

        // �������ֳɹ�
        //--------------------------------------------------------------------------------------
        else if ((pstr = (char *)os_strstr(pbuffer, "ping success")) != NULL)
        {
            ESP_DBG("ping success\n");
            ping_status = 1;	// ����״̬��1��ping success(�����ɹ�)	0����ƽ̨δӦ����������
        }


        else if ((pstr = (char *)os_strstr(pbuffer, "send message success")) != NULL)
        {}	// ��ִ���κβ���


        // ���������豸�������������ʱ�䣬�����ƻظ�������˴�
        //-----------------------------------------------------------------
        else if ((pstr = (char *)os_strstr(pbuffer, "timers")) != NULL)
        {
            user_platform_timer_start(pusrdata , pespconn);
        }

#elif SENSOR_DEVICE
        else if ((pstr = (char *)os_strstr(pbuffer, "\"status\":")) != NULL) {
            if (os_strncmp(pstr + 10, "200", 3) != 0) {
                ESP_DBG("message upload failed.\n");
            } else {
                count++;
                ESP_DBG("message upload sucessful.\n");
            }

            os_timer_disarm(&client_timer);
            os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_discon, pespconn);
            os_timer_arm(&client_timer, 10, 0);
        }

#endif

        //���������豸�����������ʶ��ݣ������ƻظ��������˴�
        //---------------------------------------------------------------
        else if ((pstr = (char *)os_strstr(pbuffer, "device")) != NULL)
        {
#if PLUG_DEVICE || LIGHT_DEVICE
            user_platform_timer_get(pespconn);	// �����������ʱ��
#elif SENSOR_DEVICE

#endif
        }

        os_memset(pbuffer,0,sizeof(pbuffer));	// �������������ݵĿռ���0
    }

#if (PLUG_DEVICE || LIGHT_DEVICE)
    os_timer_arm(&beacon_timer, BEACON_TIME, 0);	// ��ʱ50��(������ʱ��)
#endif
}


/******************************************************************************
 * FunctionName : user_esp_platform_ap_change
 * Description  : add the user interface for changing to next ap ID.
 * Parameters   :
 * Returns      : none
*******************************************************************************/

#if AP_CACHE

// (��ʱ5��)������һ����¼��WIFI�����Ҽ��IP��ȡ���
//=======================================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_ap_change(void)		// AP�л�
{
    uint8 current_id;
    uint8 i = 0;
    ESP_DBG("user_esp_platform_ap_is_changing\n");

    current_id = wifi_station_get_current_ap_id();		// ��ȡ��ǰ���ӵ�AP_id
    ESP_DBG("current ap id =%d\n", current_id);

    if (current_id == AP_CACHE_NUMBER - 1) 	// 0��4
    { i = 0; }
    else
    { i = current_id + 1; }

    while (wifi_station_ap_change(i) != true)	// ������һ��AP
    {
       i++;
       if (i == AP_CACHE_NUMBER - 1) {
    	   i = 0;
       }
    }

    device_recon_count = 0;		// ESP8266_TCP�������� = 0

    /* just need to re-check ip while change AP */
    os_timer_disarm(&client_timer);
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_check_ip, NULL);		// ��ѯESP8266��IP��ȡ���
    os_timer_arm(&client_timer, 100, 0);
}
#endif
//=======================================================================================================================

// ESP8266��ΪAP+STAģʽ������5���л�WIFI
//==============================================================================================================
LOCAL bool ICACHE_FLASH_ATTR user_esp_platform_reset_mode(void)
{
    if (wifi_get_opmode() == STATION_MODE)	// ESP8266��ΪAP+STAģʽ
    {
        wifi_set_opmode(STATIONAP_MODE);	// ������APģʽʹΪ��ʹ��APP����ESP8266����·����WIFI��SSID��PASS��
    }

#if AP_CACHE

    /* delay 5s to change AP */ // 5���������һ����¼��WIFI
    //-----------------------------------------------------------------------------------
    os_timer_disarm(&client_timer);
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_ap_change, NULL);
    os_timer_arm(&client_timer, 5000, 0);
    return true;

#endif

    return false;
}
//==============================================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_recon_cb
 * Description  : The connection had an error and is already deallocated.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/

// ��ESP8266��ΪTCP_Client������TCP_Serverʧ�� / TCP�����쳣�Ͽ����Ļص�����
//=================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_recon_cb(void *arg, sint8 err)
{
    struct espconn *pespconn = (struct espconn *)arg;

    ESP_DBG("user_esp_platform_recon_cb\n");

#if (PLUG_DEVICE || LIGHT_DEVICE)
    os_timer_disarm(&beacon_timer);		// �ر�������ʱ��
#endif

#if (PLUG_DEVICE || SENSOR_DEVICE)
    user_link_led_output(1);
#endif

    if (++device_recon_count == 5)		/// ESP8266_TCP����ʧ��5��
    {
        device_status = DEVICE_CONNECT_SERVER_FAIL;		// ״̬��Ϊ���豸���ӷ�����ʧ�ܡ�

        if (user_esp_platform_reset_mode())		// ESP8266��ΪAP+STAģʽ������5���л�WIFI
        {
            return;
        }
    }

#if SENSOR_DEVICE
#ifdef SENSOR_DEEP_SLEEP

    if (wifi_get_opmode() == STATION_MODE) {
        user_esp_platform_reset_mode();

        //user_sensor_deep_sleep_enter();
    } else {
        os_timer_disarm(&client_timer);
        os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_reconnect, pespconn);
        os_timer_arm(&client_timer, 1000, 0);
    }

#else
    os_timer_disarm(&client_timer);
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_reconnect, pespconn);
    os_timer_arm(&client_timer, 1000, 0);
#endif
#else
    os_timer_disarm(&client_timer);
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_reconnect, pespconn);	// TCP���������IP��DNS������TCP�������á�
    os_timer_arm(&client_timer, 1000, 0);
#endif
}
//=================================================================================================

/******************************************************************************
 * FunctionName : user_esp_platform_connect_cb
 * Description  : A new incoming connection has been connected.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/

// TCP���ӳɹ�_�ص�����
//==================================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;

    ESP_DBG("user_esp_platform_connect_cb\n");

    // ������������TCP���ӽ������Ǿ͹ر�softAPģʽ
    //-----------------------------------------------
    if (wifi_get_opmode() ==  STATIONAP_MODE )	// ������APģʽʹΪ��ʹ��APP����ESP8266����·����WIFI��SSID��PASS��
    {
        wifi_set_opmode(STATION_MODE);		// ��ΪSTAģʽ
    }

#if (PLUG_DEVICE || SENSOR_DEVICE)
    user_link_led_timer_done();
#endif

    device_recon_count = 0;				// ESP8266_TCP�������� = 0

    espconn_regist_recvcb(pespconn, user_esp_platform_recv_cb);		// �������ݽ��ճɹ�_�ص�����
    espconn_regist_sentcb(pespconn, user_esp_platform_sent_cb);		// �������ݷ��ͳɹ�_�ص�����

    user_esp_platform_sent(pespconn);	// ESP8266����TCP���ݣ��豸��������ƽ̨�������ݰ���
}
//==================================================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_connect
 * Description  : The function given as the connect with the host
 * Parameters   : espconn -- the espconn used to connect the connection
 * Returns      : none
*******************************************************************************/

// ESP8266(Client)����TCP Server
//==================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_connect(struct espconn *pespconn)
{
    ESP_DBG("user_esp_platform_connect\n");		// ���ڴ�ӡ�������������Ƶ��ַ���

#ifdef CLIENT_SSL_ENABLE
    espconn_secure_connect(pespconn);
#else
    espconn_connect(pespconn);		// ESP8266(Client)����TCP Server
#endif
}
//==================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_dns_found
 * Description  : dns found callback
 * Parameters   : name -- pointer to the name that was looked up.
 *                ipaddr -- pointer to an ip_addr_t containing the IP address of
 *                the hostname, or NULL if the name could not be found (or on any
 *                other error).
 *                callback_arg -- a user-specified callback argument passed to
 *                dns_gethostbyname
 * Returns      : none
*******************************************************************************/

#ifdef USE_DNS

// ���������ɹ��Ļص�������TCP���ӡ�
//============================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_dns_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
    struct espconn *pespconn = (struct espconn *)arg;

    // ���������������Σ��ɹ�����WIFI�����ǲ������������Խ�������ʧ�ܡ�
    //---------------------------------------------------------------------
    if (ipaddr == NULL)
    {
        ESP_DBG("user_esp_platform_dns_found NULL\n");

        if (++device_recon_count == 5)	// �����������=5������Ϊ����ʧ��
        {
            device_status = DEVICE_CONNECT_SERVER_FAIL;

            user_esp_platform_reset_mode();		// ESP8266��ΪAP+STAģʽ������5���л�WIFI
        }

        return;
    }

    // ���ڴ�ӡ������ƽ̨��IP��ַ
    //-----------------------------------------------------------------
    ESP_DBG("user_esp_platform_dns_found %d.%d.%d.%d\n",
            *((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr+1),
            *((uint8 *)&ipaddr->addr+2), *((uint8 *)&ipaddr->addr+3));

    // �ɹ���ȡ�����Ƶ�IP��ַ
    //-------------------------------------------------------------------------
    if (esp_server_ip.addr == 0 && ipaddr->addr != 0)
    {
        os_timer_disarm(&client_timer);		// Client��ʱ���ر�

        esp_server_ip.addr = ipaddr->addr;								// IP��ַ��ֵ
        os_memcpy(pespconn->proto.tcp->remote_ip, &ipaddr->addr, 4);	// ���÷�������ַ
        pespconn->proto.tcp->local_port = espconn_port();				// ���ñ��ض˿�


#ifdef CLIENT_SSL_ENABLE
        pespconn->proto.tcp->remote_port = 8443;	// ����Socket��		tcp://��iot.espressif.cn����iot.espressif.com��:8443
#else
        pespconn->proto.tcp->remote_port = 8000;	// ������Socket��	tcp://��iot.espressif.cn����iot.espressif.com��:8000
#endif

#if (PLUG_DEVICE || LIGHT_DEVICE)
        ping_status = 1;	// ����״̬��1��ping success(�����ɹ�)	0����ƽ̨δӦ����������
#endif

        espconn_regist_connectcb(pespconn, user_esp_platform_connect_cb);	// TCP���ӳɹ�_�ص�����
        espconn_regist_disconcb(pespconn, user_esp_platform_discon_cb);		// TCP�Ͽ��ɹ�_�ص�����
        espconn_regist_reconcb(pespconn, user_esp_platform_recon_cb);		// TCP�쳣�Ͽ�_�ص�����

        user_esp_platform_connect(pespconn);	// ESP8266(Client)����TCP Server
    }
}
//============================================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_dns_check_cb
 * Description  : 1s time callback to check dns found
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/

// DNS����
//========================================================================================================================
LOCAL void ICACHE_FLASH_ATTR user_esp_platform_dns_check_cb(void *arg)	// ��ʱ��������Ƿ�����ɹ�
{
    struct espconn *pespconn = arg;

    ESP_DBG("user_esp_platform_dns_check_cb\n");

    espconn_gethostbyname(pespconn, ESP_DOMAIN, &esp_server_ip, user_esp_platform_dns_found);	// ��������

    os_timer_arm(&client_timer, 1000, 0);	// ��ʱ��ѯ�����������
}

LOCAL void ICACHE_FLASH_ATTR user_esp_platform_start_dns(struct espconn *pespconn)
{
    esp_server_ip.addr = 0;
    espconn_gethostbyname(pespconn, ESP_DOMAIN, &esp_server_ip, user_esp_platform_dns_found);	// ��������ƽ̨��

    os_timer_disarm(&client_timer);		// ����Client��ʱ��
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_dns_check_cb, pespconn);	// ��ʱ��ѯ�����������
    os_timer_arm(&client_timer, 1000, 0);
}
#endif
//========================================================================================================================

// mDNS����
//=======================================================================================
#if LIGHT_DEVICE
void user_mdns_conf()
{
	struct ip_info ipconfig;

	wifi_get_ip_info(STATION_IF, &ipconfig);	// ��ȡSTAģʽ�µ�IP��ַ

	struct mdns_info *info = (struct mdns_info *)os_zalloc(sizeof(struct mdns_info));
	info->host_name = "espressif_light_demo";
	info->ipAddr= ipconfig.ip.addr; //sation ip
	info->server_name = "espLight";
	info->server_port = 80;
	info->txt_data[0] = "version = 1.0.1";
	espconn_mdns_init(info);					// ��ʼ��mDNS
}
#endif
//=======================================================================================

/******************************************************************************
 * FunctionName : user_esp_platform_check_ip
 * Description  : espconn struct parame init when get ip addr
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
// ��ѯESP8266��IP��ȡ�����DNS������TCP���á�
//==================================================================================================================================
void ICACHE_FLASH_ATTR user_esp_platform_check_ip(uint8 reset_flag)
{
    struct ip_info ipconfig;					// IP��Ϣ�ṹ��

    os_timer_disarm(&client_timer);				// ��ʱ�ر�Client��ʱ��

    wifi_get_ip_info(STATION_IF, &ipconfig);	// ��ȡSTAģʽ�µ�IP��ַ

    // ESP8266��ȡ��IP��ַ
    //---------------------------------------------------------------------------
    if (wifi_station_get_connect_status()==STATION_GOT_IP && ipconfig.ip.addr!=0)
    {
#if (PLUG_DEVICE || SENSOR_DEVICE)
        user_link_led_timer_init();
#endif

//--------------------------------------
#if LIGHT_DEVICE
	user_mdns_conf();	// mDNS��ʼ��
#endif
//--------------------------------------

		// TCP��������
		//------------------------------------------------------------------
        user_conn.proto.tcp = &user_tcp;	// �����ڴ�
        user_conn.type = ESPCONN_TCP;		// ��ΪTCP����
        user_conn.state = ESPCONN_NONE;

        device_status = DEVICE_CONNECTING;	// �豸(8266)״̬��Ϊ���������ӡ�

        if (reset_flag)				// �ж��Ƿ��Ǹո�λ
        { device_recon_count = 0;}	// ϵͳ��λ��ESP8266_TCP��������=0��reset_flag==1��ʾΪϵͳ��λ��

#if (PLUG_DEVICE || LIGHT_DEVICE)
        os_timer_disarm(&beacon_timer);	// ����������ʱ����ʱ��
        os_timer_setfn(&beacon_timer, (os_timer_func_t *)user_esp_platform_sent_beacon, &user_conn);
#endif

#ifdef USE_DNS
        user_esp_platform_start_dns(&user_conn);	// DNS����
#else
        const char esp_server_ip[4] = {114, 215, 177, 97};

        os_memcpy(user_conn.proto.tcp->remote_ip, esp_server_ip, 4);
        user_conn.proto.tcp->local_port = espconn_port();

#ifdef CLIENT_SSL_ENABLE
        user_conn.proto.tcp->remote_port = 8443;
#else
        user_conn.proto.tcp->remote_port = 8000;
#endif

        espconn_regist_connectcb(&user_conn, user_esp_platform_connect_cb);
        espconn_regist_reconcb(&user_conn, user_esp_platform_recon_cb);
        user_esp_platform_connect(&user_conn);
#endif
    }

    // δ��ȡ��IP��ַ
    //------------------------------------------------------------------------------------------
    else
    {
        /* if there are wrong while connecting to some AP, then reset mode */
        if ((wifi_station_get_connect_status() == STATION_WRONG_PASSWORD ||		// [�������]��[δ����AP]��[����ʧ��]
                wifi_station_get_connect_status() == STATION_NO_AP_FOUND ||
                wifi_station_get_connect_status() == STATION_CONNECT_FAIL))
        {
            user_esp_platform_reset_mode();		// ESP8266��ΪAP+STAģʽ������5���л�WIFI
        }
        else
        {
            os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_check_ip, NULL);	// ������ѯESP8266��IP��ȡ���
            os_timer_arm(&client_timer, 100, 0);
        }
    }
}
//==================================================================================================================================


/******************************************************************************
 * FunctionName : user_esp_platform_init
 * Description  : device parame init based on espressif platform
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
// ��ѯ��λ״̬��������ʼ��
//==================================================================================================================================
void ICACHE_FLASH_ATTR user_esp_platform_init(void)
{
	// ��ȡ�汾��Ϣ
	//--------------------------------------------------------------------------
	os_sprintf(iot_version,"%s%d.%d.%dt%d(%s)",VERSION_TYPE,IOT_VERSION_MAJOR,\
	IOT_VERSION_MINOR,IOT_VERSION_REVISION,device_type,UPGRADE_FALG);
	os_printf("IOT VERSION = %s\n",iot_version);

	//--------------------------------------------------------------------------------------------------------------------
	system_param_load(priv_param_start_sec+1,0,&esp_param,sizeof(esp_param));	// ��ȡ��0x7D(0x7C+1)������������(KEY_BIN)
	os_printf("esp_param.devkey = %s\n",esp_param.devkey);						// ���ڴ�ӡ��devkey��
	os_printf("esp_param.token = %s\n",esp_param.token);						// ���ڴ�ӡ��token��
	os_printf("esp_param.pad = %s\n",esp_param.pad);							// ���ڴ�ӡ��pad��
	os_printf("esp_param.activeflag = %d\n",esp_param.activeflag);				// ���ڴ�ӡ��activeflag��

//	while(1) system_soft_wdt_feed();	// ����С����ӡ�

	// ESP8266��λ��ִ�и�λ��ѯ
	//-------------------------------------------------------------------------
	struct rst_info *rtc_info = system_get_rst_info();	// ��ȡ��ǰ��������Ϣ
	os_printf("reset reason: %x\n", rtc_info->reason);	// ��ӡ��λԭ��

	// �жϸ�λԭ��
	//--------------------------------------------------------------------
	if (rtc_info->reason == REASON_WDT_RST ||			// ���Ź���λ
		rtc_info->reason == REASON_EXCEPTION_RST ||		// �쳣��λ
		rtc_info->reason == REASON_SOFT_WDT_RST)		// ������Ź���λ
	{
		if (rtc_info->reason == REASON_EXCEPTION_RST)
		{
			os_printf("Fatal exception (%d):\n", rtc_info->exccause);
		}
		os_printf("epc1=0x%08x, epc2=0x%08x, epc3=0x%08x, excvaddr=0x%08x, depc=0x%08x\n",
		rtc_info->epc1, rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr, rtc_info->depc);
	}


	// ����֮ǰ��IP��ַ
	//-------------------------------------------------------------------------
	/***add by tzx for saving ip_info to avoid dhcp_client start****/
    struct dhcp_client_info dhcp_info;
    struct ip_info sta_info;
    system_rtc_mem_read(64,&dhcp_info,sizeof(struct dhcp_client_info));	// ��ȡRTC memory�е�����

    // �ж�֮ǰ�Ƿ񱣴�Ϊ1
    //-----------------------------------------------------------------
	if(dhcp_info.flag == 0x01 )
	{
		if (true == wifi_station_dhcpc_status())	// STA_DHCP����
		{
			wifi_station_dhcpc_stop();				// STA_DHCPֹͣ
		}

		sta_info.ip = dhcp_info.ip_addr;	// ������Ϊ֮ǰ��IP��ַ
		sta_info.gw = dhcp_info.gw;
		sta_info.netmask = dhcp_info.netmask;
		if ( true != wifi_set_ip_info(STATION_IF,&sta_info))	// ����STA��IP��ַ
		{ os_printf("set default ip wrong\n"); }
	}

    os_memset(&dhcp_info,0,sizeof(struct dhcp_client_info));	 // dhcp_info��0
    system_rtc_mem_write(64,&dhcp_info,sizeof(struct rst_info));// RTC_mem��0


#if AP_CACHE
    wifi_station_ap_number_set(AP_CACHE_NUMBER);	// AP��Ϣ����(5��)
#endif


#if 0
    {
        char sofap_mac[6] = {0x16, 0x34, 0x56, 0x78, 0x90, 0xab};
        char sta_mac[6] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};
        struct ip_info info;

        wifi_set_macaddr(SOFTAP_IF, sofap_mac);
        wifi_set_macaddr(STATION_IF, sta_mac);

        IP4_ADDR(&info.ip, 192, 168, 3, 200);
        IP4_ADDR(&info.gw, 192, 168, 3, 1);
        IP4_ADDR(&info.netmask, 255, 255, 255, 0);
        wifi_set_ip_info(STATION_IF, &info);

        IP4_ADDR(&info.ip, 10, 10, 10, 1);
        IP4_ADDR(&info.gw, 10, 10, 10, 1);
        IP4_ADDR(&info.netmask, 255, 255, 255, 0);
        wifi_set_ip_info(SOFTAP_IF, &info);
    }
#endif


    // esp_param.activeflag ==��0�����ƶ��豸δ�����ʼֵ==0xFF��
    // esp_param.activeflag ==��1�����ƶ��豸�Ѽ���
    //------------------------------------------------------------------
    if (esp_param.activeflag != 1)		// ���ƶ��豸��δ����
    {
#ifdef SOFTAP_ENCRYPT
        struct softap_config config;
        char password[33];
        char macaddr[6];

        wifi_softap_get_config(&config);
        wifi_get_macaddr(SOFTAP_IF, macaddr);

        os_memset(config.password, 0, sizeof(config.password));
        os_sprintf(password, MACSTR "_%s", MAC2STR(macaddr), PASSWORD);
        os_memcpy(config.password, password, os_strlen(password));
        config.authmode = AUTH_WPA_WPA2_PSK;

        wifi_softap_set_config(&config);
#endif

        wifi_set_opmode(STATIONAP_MODE);	// ��ΪAP+STAģʽ������APģʽʹΪ��ʹ��APP����ESP8266����·����WIFI��SSID��PASS��
    }

#if PLUG_DEVICE
    user_plug_init();	// ������ʼ��
#elif LIGHT_DEVICE
    user_light_init();	// �ƹ��ʼ��(PWM)
#elif SENSOR_DEVICE
    user_sensor_init(esp_param.activeflag);	// ��������ʼ��
#endif

    // �ж�ESP8266�Ƿ�ΪSoftAPģʽ
    //-------------------------------------------------------------------------------------------
    if (wifi_get_opmode() != SOFTAP_MODE)	// ����SoftAPģʽ
    {
    	// ���ö�ʱ������ʱ��ѯESP8266��IP�����
    	//----------------------------------------
    	os_timer_disarm(&client_timer);
        os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_check_ip,(void *)1);	// ����3=1����ʾ��ǰ�Ǹո�λ״̬
        os_timer_arm(&client_timer, 100, 0);	// ʹ�ܺ��붨ʱ��(100Ms��ʱ)
    }
}
//==================================================================================================================================

#endif
