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

#ifndef __ESPCONN_H__
#define __ESPCONN_H__

typedef sint8 err_t;

typedef void *espconn_handle;
typedef void (* espconn_connect_callback)(void *arg);
typedef void (* espconn_reconnect_callback)(void *arg, sint8 err);

/* Definitions for error constants. */

#define ESPCONN_OK          0    /* No error, everything OK. */
#define ESPCONN_MEM        -1    /* Out of memory error.     */
#define ESPCONN_TIMEOUT    -3    /* Timeout.                 */
#define ESPCONN_RTE        -4    /* Routing problem.         */
#define ESPCONN_INPROGRESS  -5    /* Operation in progress    */
#define ESPCONN_MAXNUM		-7	 /* Total number exceeds the set maximum*/

#define ESPCONN_ABRT       -8    /* Connection aborted.      */
#define ESPCONN_RST        -9    /* Connection reset.        */
#define ESPCONN_CLSD       -10   /* Connection closed.       */
#define ESPCONN_CONN       -11   /* Not connected.           */

#define ESPCONN_ARG        -12   /* Illegal argument.        */
#define ESPCONN_IF		   -14	 /* UDP send error			 */
#define ESPCONN_ISCONN     -15   /* Already connected.       */

#define ESPCONN_HANDSHAKE  -28   /* ssl handshake failed	 */
#define ESPCONN_SSL_INVALID_DATA  -61   /* ssl application invalid	 */

/** Protocol family and type of the espconn */
enum espconn_type {
    ESPCONN_INVALID    = 0,
    /* ESPCONN_TCP Group */
    ESPCONN_TCP        = 0x10,
    /* ESPCONN_UDP Group */
    ESPCONN_UDP        = 0x20,
};

/** Current state of the espconn. Non-TCP espconn are always in state ESPCONN_NONE! */
enum espconn_state {
    ESPCONN_NONE,
    ESPCONN_WAIT,
    ESPCONN_LISTEN,
    ESPCONN_CONNECT,
    ESPCONN_WRITE,
    ESPCONN_READ,
    ESPCONN_CLOSE
};

typedef struct _esp_tcp {
    int remote_port;
    int local_port;
    uint8 local_ip[4];
    uint8 remote_ip[4];
    espconn_connect_callback connect_callback;
    espconn_reconnect_callback reconnect_callback;
    espconn_connect_callback disconnect_callback;
	espconn_connect_callback write_finish_fn;
} esp_tcp;

typedef struct _esp_udp {
    int remote_port;
    int local_port;
    uint8 local_ip[4];
	uint8 remote_ip[4];
} esp_udp;

typedef struct _remot_info{
	enum espconn_state state;
	int remote_port;
	uint8 remote_ip[4];
}remot_info;

/** A callback prototype to inform about events for a espconn */
typedef void (* espconn_recv_callback)(void *arg, char *pdata, unsigned short len);
typedef void (* espconn_sent_callback)(void *arg);

/** A espconn descriptor */
struct espconn {
    /** type of the espconn (TCP, UDP) */
    enum espconn_type type;
    /** current state of the espconn */
    enum espconn_state state;
    union {
        esp_tcp *tcp;
        esp_udp *udp;
    } proto;
    /** A callback function that is informed about events for this espconn */
    espconn_recv_callback recv_callback;
    espconn_sent_callback sent_callback;
    uint8 link_cnt;
    void *reverse;
};

enum espconn_option{
	ESPCONN_START = 0x00,
	ESPCONN_REUSEADDR = 0x01,
	ESPCONN_NODELAY = 0x02,
	ESPCONN_COPY = 0x04,
	ESPCONN_KEEPALIVE = 0x08,
	ESPCONN_MANUALRECV     = 0x10,
	ESPCONN_END
};

enum espconn_level{
	ESPCONN_KEEPIDLE,
	ESPCONN_KEEPINTVL,
	ESPCONN_KEEPCNT
};

enum {
	ESPCONN_IDLE = 0,
	ESPCONN_CLIENT,
	ESPCONN_SERVER,
	ESPCONN_BOTH,
	ESPCONN_MAX
};

struct espconn_packet{
	uint16 sent_length;		/* sent length successful*/
	uint16 snd_buf_size;	/* Available buffer size for sending  */
	uint16 snd_queuelen;	/* Available buffer space for sending */
	uint16 total_queuelen;	/* total Available buffer space for sending */
	uint32 packseqno;		/* seqno to be sent */
	uint32 packseq_nxt;		/* seqno expected */
	uint32 packnum;
};

// mDNS信息
//-----------------------------------------
struct mdns_info {
	char *host_name;		// 主机名
	char *server_name;		// 服务器名
	uint16 server_port;		// 服务器端口
	unsigned long ipAddr;	// IP地址
	char *txt_data[10];		// 文本字符串
};
//-----------------------------------------

/******************************************************************************
 * FunctionName : espconn_connect
 * Description  : The function given as the connect
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_connect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_disconnect
 * Description  : disconnect with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_disconnect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_delete
 * Description  : disconnect with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_delete(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_accept
 * Description  : The function given as the listen
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_accept(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_create
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to the data transmission
 * Returns      : result
*******************************************************************************/

sint8 espconn_create(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_con
 * Description  : get the number of simulatenously active TCP connections
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

uint8 espconn_tcp_get_max_con(void);

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con
 * Description  : set the number of simulatenously active TCP connections
 * Parameters   : num -- total number
 * Returns      : none
*******************************************************************************/

sint8 espconn_tcp_set_max_con(uint8 num);

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_con_allow
 * Description  : get the count of simulatenously active connections on the server
 * Parameters   : espconn -- espconn to get the count
 * Returns      : result
*******************************************************************************/

sint8 espconn_tcp_get_max_con_allow(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con_allow
 * Description  : set the count of simulatenously active connections on the server
 * Parameters   : espconn -- espconn to set the count
 * 				  num -- support the connection number
 * Returns      : result
*******************************************************************************/

sint8 espconn_tcp_set_max_con_allow(struct espconn *espconn, uint8 num);

/******************************************************************************
 * FunctionName : espconn_regist_time
 * Description  : used to specify the time that should be called when don't recv data
 * Parameters   : espconn -- the espconn used to the connection
 * 				  interval -- the timer when don't recv data
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_time(struct espconn *espconn, uint32 interval, uint8 type_flag);

/******************************************************************************
 * FunctionName : espconn_get_connection_info
 * Description  : used to specify the function that should be called when disconnect
 * Parameters   : espconn -- espconn to set the err callback
 *                discon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/

sint8 espconn_get_connection_info(struct espconn *pespconn, remot_info **pcon_info, uint8 typeflags);

/******************************************************************************
 * FunctionName : espconn_get_packet_info
 * Description  : get the packet info with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * 				  infoarg -- the packet info
 * Returns      : the errur code
*******************************************************************************/

sint8 espconn_get_packet_info(struct espconn *espconn, struct espconn_packet* infoarg);

/******************************************************************************
 * FunctionName : espconn_regist_sentcb
 * Description  : Used to specify the function that should be called when data
 *                has been successfully delivered to the remote host.
 * Parameters   : struct espconn *espconn -- espconn to set the sent callback
 *                espconn_sent_callback sent_cb -- sent callback function to
 *                call for this espconn when data is successfully sent
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_sentcb(struct espconn *espconn, espconn_sent_callback sent_cb);

/******************************************************************************
 * FunctionName : espconn_regist_sentcb
 * Description  : Used to specify the function that should be called when data
 *                has been successfully delivered to the remote host.
 * Parameters   : espconn -- espconn to set the sent callback
 *                sent_cb -- sent callback function to call for this espconn
 *                when data is successfully sent
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_write_finish(struct espconn *espconn, espconn_connect_callback write_finish_fn);

/******************************************************************************
 * FunctionName : espconn_send
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_send(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_sent
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_sent(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_sendto
 * Description  : send data for UDP
 * Parameters   : espconn -- espconn to set for UDP
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : error
*******************************************************************************/

sint16 espconn_sendto(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_regist_connectcb
 * Description  : used to specify the function that should be called when
 *                connects to host.
 * Parameters   : espconn -- espconn to set the connect callback
 *                connect_cb -- connected callback function to call when connected
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_connectcb(struct espconn *espconn, espconn_connect_callback connect_cb);

/******************************************************************************
 * FunctionName : espconn_regist_recvcb
 * Description  : used to specify the function that should be called when recv
 *                data from host.
 * Parameters   : espconn -- espconn to set the recv callback
 *                recv_cb -- recv callback function to call when recv data
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_recvcb(struct espconn *espconn, espconn_recv_callback recv_cb);

/******************************************************************************
 * FunctionName : espconn_regist_reconcb
 * Description  : used to specify the function that should be called when connection
 *                because of err disconnect.
 * Parameters   : espconn -- espconn to set the err callback
 *                recon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_reconcb(struct espconn *espconn, espconn_reconnect_callback recon_cb);

/******************************************************************************
 * FunctionName : espconn_regist_disconcb
 * Description  : used to specify the function that should be called when disconnect
 * Parameters   : espconn -- espconn to set the err callback
 *                discon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_disconcb(struct espconn *espconn, espconn_connect_callback discon_cb);

/******************************************************************************
 * FunctionName : espconn_port
 * Description  : access port value for client so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : none
 * Returns      : access port value
*******************************************************************************/

uint32 espconn_port(void);

/******************************************************************************
 * FunctionName : espconn_set_opt
 * Description  : access port value for client so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : none
 * Returns      : access port value
*******************************************************************************/

sint8 espconn_set_opt(struct espconn *espconn, uint8 opt);

/******************************************************************************
 * FunctionName : espconn_clear_opt
 * Description  : clear the option for connections so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : espconn -- the espconn used to set the connection
 * 				  opt -- the option for clear
 * Returns      : the result
*******************************************************************************/

sint8 espconn_clear_opt(struct espconn *espconn, uint8 opt);

/******************************************************************************
 * FunctionName : espconn_set_keepalive
 * Description  : access level value for connection so that we set the value for
 * 				  keep alive
 * Parameters   : espconn -- the espconn used to set the connection
 * 				  level -- the connection's level
 * 				  value -- the value of time(s)
 * Returns      : access port value
*******************************************************************************/

sint8 espconn_set_keepalive(struct espconn *espconn, uint8 level, void* optarg);

/******************************************************************************
 * FunctionName : espconn_get_keepalive
 * Description  : access level value for connection so that we get the value for
 * 				  keep alive
 * Parameters   : espconn -- the espconn used to get the connection
 * 				  level -- the connection's level
 * Returns      : access keep alive value
*******************************************************************************/

sint8 espconn_get_keepalive(struct espconn *espconn, uint8 level, void *optarg);

/******************************************************************************
 * TypedefName : dns_found_callback
 * Description : Callback which is invoked when a hostname is found.
 * Parameters  : name -- pointer to the name that was looked up.
 *               ipaddr -- pointer to an ip_addr_t containing the IP address of
 *               the hostname, or NULL if the name could not be found (or on any
 *               other error).
 *               callback_arg -- a user-specified callback argument passed to
 *               dns_gethostbyname
*******************************************************************************/

typedef void (*dns_found_callback)(const char *name, ip_addr_t *ipaddr, void *callback_arg);

/******************************************************************************
 * FunctionName : espconn_gethostbyname
 * Description  : Resolve a hostname (string) into an IP address.
 * Parameters   : pespconn -- espconn to resolve a hostname
 *                hostname -- the hostname that is to be queried
 *                addr -- pointer to a ip_addr_t where to store the address if 
 *                it is already cached in the dns_table (only valid if ESPCONN_OK
 *                is returned!)
 *                found -- a callback function to be called on success, failure
 *                or timeout (only if ERR_INPROGRESS is returned!)
 * Returns      : err_t return code
 *                - ESPCONN_OK if hostname is a valid IP address string or the host
 *                  name is already in the local names table.
 *                - ESPCONN_INPROGRESS enqueue a request to be sent to the DNS server
 *                  for resolution if no errors are present.
 *                - ESPCONN_ARG: dns client not initialized or invalid hostname
*******************************************************************************/

err_t espconn_gethostbyname(struct espconn *pespconn, const char *hostname, ip_addr_t *addr, dns_found_callback found);

/******************************************************************************
 * FunctionName : espconn_abort
 * Description  : Forcely abort with host
 * Parameters   : espconn -- the espconn used to connect with the host
 * Returns      : result
*******************************************************************************/

sint8 espconn_abort(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_encry_connect
 * Description  : The function given as connection
 * Parameters   : espconn -- the espconn used to connect with the host
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_connect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_encry_disconnect
 * Description  : The function given as the disconnection
 * Parameters   : espconn -- the espconn used to disconnect with the host
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_disconnect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_secure_send
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 * 				  psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_send(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_encry_sent
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 * 				  psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_sent(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_secure_set_size
 * Description  : set the buffer size for client or server
 * Parameters   : level -- set for client or server
 * 				  1: client,2:server,3:client and server
 * 				  size -- buffer size
 * Returns      : true or false
*******************************************************************************/

bool espconn_secure_set_size(uint8 level, uint16 size);

/******************************************************************************
 * FunctionName : espconn_secure_get_size
 * Description  : get buffer size for client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 * Returns      : buffer size for client or server
*******************************************************************************/

sint16 espconn_secure_get_size(uint8 level);

/******************************************************************************
 * FunctionName : espconn_secure_ca_enable
 * Description  : enable the certificate authenticate and set the flash sector
 * 				  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 *				  flash_sector -- flash sector for save certificate
 * Returns      : result true or false
*******************************************************************************/

bool espconn_secure_ca_enable(uint8 level, uint32 flash_sector );

/******************************************************************************
 * FunctionName : espconn_secure_ca_disable
 * Description  : disable the certificate authenticate  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 * Returns      : result true or false
*******************************************************************************/

bool espconn_secure_ca_disable(uint8 level);


/******************************************************************************
 * FunctionName : espconn_secure_cert_req_enable
 * Description  : enable the client certificate authenticate and set the flash sector
 * 				  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 *				  flash_sector -- flash sector for save certificate
 * Returns      : result true or false
*******************************************************************************/

bool espconn_secure_cert_req_enable(uint8 level, uint32 flash_sector );

/******************************************************************************
 * FunctionName : espconn_secure_ca_disable
 * Description  : disable the client certificate authenticate  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 * Returns      : result true or false
*******************************************************************************/

bool espconn_secure_cert_req_disable(uint8 level);

/******************************************************************************
 * FunctionName : espconn_secure_set_default_certificate
 * Description  : Load the certificates in memory depending on compile-time
 * 				  and user options.
 * Parameters   : certificate -- Load the certificate
 *				  length -- Load the certificate length
 * Returns      : result true or false
*******************************************************************************/

bool espconn_secure_set_default_certificate(const uint8* certificate, uint16 length);

/******************************************************************************
 * FunctionName : espconn_secure_set_default_private_key
 * Description  : Load the key in memory depending on compile-time
 * 				  and user options.
 * Parameters   : private_key -- Load the key
 *				  length -- Load the key length
 * Returns      : result true or false
*******************************************************************************/

bool espconn_secure_set_default_private_key(const uint8* private_key, uint16 length);

/******************************************************************************
 * FunctionName : espconn_secure_accept
 * Description  : The function given as the listen
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : result
*******************************************************************************/

sint8 espconn_secure_accept(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_secure_accepts
 * Description  : delete the secure server host
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : result
*******************************************************************************/

sint8 espconn_secure_delete(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_igmp_join
 * Description  : join a multicast group
 * Parameters   : host_ip -- the ip address of udp server
 * 				  multicast_ip -- multicast ip given by user
 * Returns      : none
*******************************************************************************/
sint8 espconn_igmp_join(ip_addr_t *host_ip, ip_addr_t *multicast_ip);

/******************************************************************************
 * FunctionName : espconn_igmp_leave
 * Description  : leave a multicast group
 * Parameters   : host_ip -- the ip address of udp server
 * 				  multicast_ip -- multicast ip given by user
 * Returns      : none
*******************************************************************************/
sint8 espconn_igmp_leave(ip_addr_t *host_ip, ip_addr_t *multicast_ip);

/******************************************************************************
 * FunctionName : espconn_recv_hold
 * Description  : hold tcp receive
 * Parameters   : espconn -- espconn to hold
 * Returns      : none
*******************************************************************************/
sint8 espconn_recv_hold(struct espconn *pespconn);

/******************************************************************************
 * FunctionName : espconn_recv_unhold
 * Description  : unhold tcp receive
 * Parameters   : espconn -- espconn to unhold
 * Returns      : none
*******************************************************************************/
sint8 espconn_recv_unhold(struct espconn *pespconn);

/******************************************************************************
 * FunctionName : espconn_mdns_init
 * Description  : register a device with mdns
 * Parameters   : ipAddr -- the ip address of device
 * 				  hostname -- the hostname of device
 * Returns      : none
*******************************************************************************/

void espconn_mdns_init(struct mdns_info *info);
/******************************************************************************
 * FunctionName : espconn_mdns_close
 * Description  : close a device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/

void espconn_mdns_close(void);
/******************************************************************************
 * FunctionName : espconn_mdns_server_register
 * Description  : register a device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
void espconn_mdns_server_register(void);

/******************************************************************************
 * FunctionName : espconn_mdns_server_unregister
 * Description  : unregister a device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
void espconn_mdns_server_unregister(void);

/******************************************************************************
 * FunctionName : espconn_mdns_get_servername
 * Description  : get server name of device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/

char* espconn_mdns_get_servername(void);
/******************************************************************************
 * FunctionName : espconn_mdns_set_servername
 * Description  : set server name of device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
void espconn_mdns_set_servername(const char *name);

/******************************************************************************
 * FunctionName : espconn_mdns_set_hostname
 * Description  : set host name of device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
void espconn_mdns_set_hostname(char *name);

/******************************************************************************
 * FunctionName : espconn_mdns_get_hostname
 * Description  : get host name of device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
char* espconn_mdns_get_hostname(void);

/******************************************************************************
 * FunctionName : espconn_mdns_disable
 * Description  : disable a device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
void espconn_mdns_disable(void);

/******************************************************************************
 * FunctionName : espconn_mdns_enable
 * Description  : disable a device with mdns
 * Parameters   : a
 * Returns      : none
*******************************************************************************/
void espconn_mdns_enable(void);
/******************************************************************************
 * FunctionName : espconn_dns_setserver
 * Description  : Initialize one of the DNS servers.
 * Parameters   : numdns -- the index of the DNS server to set must
 * 				  be < DNS_MAX_SERVERS = 2
 * 			      dnsserver -- IP address of the DNS server to set
 *  Returns     : none
*******************************************************************************/
void espconn_dns_setserver(uint8 numdns, ip_addr_t *dnsserver);
/******************************************************************************
 * FunctionName : espconn_dns_getserver
 * Description  : get dns server.
 * Parameters   : numdns -- the index of the DNS server, must
 *                be < DNS_MAX_SERVERS = 2
 *  Returns     : dnsserver -- IP address of the DNS server to set
*******************************************************************************/
ip_addr_t espconn_dns_getserver(uint8 numdns);
#endif

