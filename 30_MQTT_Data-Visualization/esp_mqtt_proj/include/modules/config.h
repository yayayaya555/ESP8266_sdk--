/* config.h
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

#include "os_type.h"
#include "user_config.h"

// 系统参数
//-------------------------------------------------------------------------------------
typedef struct{
	uint32_t cfg_holder;		// 持有人标识(只有更新此数值，系统参数才会更新)
	uint8_t device_id[64];		// 客户端ID[64]		【官方例程中是[32]，将其改为[64]】

	uint8_t sta_ssid[64];		// WIFI名[32]
	uint8_t sta_pwd[64];		// WIFI密[32]
	uint32_t sta_type;			// STA类型

	uint8_t mqtt_host[64];		// MQTT服务端域名[64]
	uint32_t mqtt_port;			// MQTT端口

	uint8_t mqtt_user[64];		// MQTT用户名[64]	【官方例程中是[32]，将其改为[64]】
	uint8_t mqtt_pass[64];		// MQTT密码[64]		【官方例程中是[32]，将其改为[64]】

	uint32_t mqtt_keepalive;	// 保持连接时长
	uint8_t security;			// 安全类型
} SYSCFG;
//-------------------------------------------------------------------------------------

// 参数扇区标志
//-------------------------------------------------------------------------------------
typedef struct {
    uint8 flag;		//【0：系统参数在0x79扇区	!0：系统参数在0x7A扇区】
    uint8 pad[3];
} SAVE_FLAG;
//-------------------------------------------------------------------------------------

void ICACHE_FLASH_ATTR CFG_Save();
void ICACHE_FLASH_ATTR CFG_Load();

extern SYSCFG sysCfg;

#endif /* USER_CONFIG_H_ */
