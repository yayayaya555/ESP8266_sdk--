
#include "modules/dht11.h"	// DHT11


// 全局变量
//==================================================================================
// DHT11_Data_Array[0] == 湿度_整数_部分
// DHT11_Data_Array[1] == 湿度_小数_部分
// DHT11_Data_Array[2] == 温度_整数_部分
// DHT11_Data_Array[3] == 温度_小数_部分
// DHT11_Data_Array[4] == 校验字节
// DHT11_Data_Array[5] == 【1:温度>=0℃】【0:温度<0℃】
//-----------------------------------------------------
u8 DHT11_Data_Array[6] = {0};	// DHT11数据数组

u8 DHT11_Data_Char[2][10]={0};	// DHT11数据字符串【行：湿/温度】【列：数据字符串】
// DHT11_Data_Char[0] == 【湿度字符串】
// DHT11_Data_Char[1] == 【温度字符串】
//==================================================================================


// 毫秒延时函数
//=================================================
void ICACHE_FLASH_ATTR Dht11_delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//=================================================


// GPIO_5(DHT11信号线)设为输出模式，并输出参数对应的电平
//===========================================================================
void ICACHE_FLASH_ATTR DHT11_Signal_Output(u8 Value_Vol)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,	FUNC_GPIO5);	// GPIO5设为IO口
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5),Value_Vol);				// IO5设为输出=X
}
//===========================================================================


// GPIO_5(DHT11信号线)设为输入模式
//===========================================================================
void ICACHE_FLASH_ATTR DHT11_Signal_Input(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,	FUNC_GPIO5);	// GPIO5设为IO口
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(5));		// GPIO5失能输出(输入)
}
//===========================================================================


// DHT11：输出起始信号－＞接收响应信号
//-----------------------------------------------------
// 返回值：		0		成功
//				1		失败：规定时间内未接收到响应信号
//				2		失败：响应信号的低电平时长超时
//===========================================================================
u8 ICACHE_FLASH_ATTR DHT11_Start_Signal_JX(void)
{
	u8 C_delay_time = 0;	// 延时计时

	// IO5抬高
	//--------------------------------------------------------
	DHT11_Signal_Output(1);	// DHT11信号线(IO5) == 输出高
	Dht11_delay_ms(1);

	// IO5拉低(25ms)：起始信号
	//---------------------------------------------
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5),0);	// IO5 = 0
	Dht11_delay_ms(25);

	// IO5抬高【注：起始信号结束后的约13us，DHT11开始输出信号】
	//---------------------------------------------------------
	GPIO_OUTPUT_SET(GPIO_ID_PIN(5),1);	// IO5 = 1
	os_delay_us(5);		// 延时5us


	// 接收响应信号
	//……………………………………………………………………………………
	// IO5设为输入：接收DHT11数据
	//-------------------------------------------------
	DHT11_Signal_Input();	// DHT11信号线(IO5) = 输入

	// 等待响应信号的低电平【最迟等50us】
	//-------------------------------------------------------------
	while( GPIO_INPUT_GET(GPIO_ID_PIN(5))==1 && C_delay_time<50 )
	{
		os_delay_us(1);		// 1us计时
		C_delay_time++;
	}

	// 响应信号超时未收到
	//--------------------------------------------------
	if(C_delay_time >= 50)
		return 1;	// 失败：规定时间内未接收到响应信号


	C_delay_time = 0 ;		// 低电平计时开始

	// 响应信息的低电平时长计时【最多170us】
	//-------------------------------------------------------------
	while( GPIO_INPUT_GET(GPIO_ID_PIN(5))==0 && C_delay_time<170 )
	{
		os_delay_us(1);
		C_delay_time++;	// 低电平时长
	}

	// 响应信号的低电平时长超时
	//------------------------------------------------
	if(C_delay_time >= 170)
		return 2;	// 失败：响应信号的低电平时长超时

	// 响应信号的低电平成功接收
	//--------------------------
	else
		return 0;	// 成功
}
//===========================================================================


// 读取DHT11一位数据
//--------------------------------
// 返回值：		0		数据=="0"
//				1		数据=="1"
//======================================================================
u8 ICACHE_FLASH_ATTR DHT11_Read_Bit(void)
{
	u8 C_delay_time = 0;	// 延时计时

	// 等待响应信息的低电平【最迟等150us】
	//-------------------------------------------------------------
	while( GPIO_INPUT_GET(GPIO_ID_PIN(5))==1 && C_delay_time<150 )
	{
		os_delay_us(1);		// 1us计时
		C_delay_time++;
	}

	C_delay_time = 0 ;		// 低电平计时开始

	// 数据位的低电平时长计时【最多200us】
	//-------------------------------------------------------------
	while( GPIO_INPUT_GET(GPIO_ID_PIN(5))==0 && C_delay_time<120 )
	{
		os_delay_us(1);
		C_delay_time++;	// 低电平时长
	}

	// 数据位的低电平结束后，是数据位的高电平
	// 数据"0"的高电平时长 == [23～27us]
	// 数据"1"的高电平时长 == [68～74us]
	//------------------------------------------------
	os_delay_us(45);	// 跳过数据"0"的高电平部分

	// 延时45us后，检测信号线电平
	// 如果此时信号线电平==1 => 数据=="1"
	// 如果此时信号线电平==0 => 数据=="0"
	//-------------------------------------
	return GPIO_INPUT_GET(GPIO_ID_PIN(5));
}
//======================================================================

// 读取DHT11一个字节
//======================================================================
u8 ICACHE_FLASH_ATTR DHT11_Read_Byte(void)
{
	u8 C_Bit = 0;	// 位计数

	u8 T_DHT11_Byte_Data = 0;	// DHT11字节数据

	for(; C_Bit<8; C_Bit++)		// 读取DHT11一个字节
	{
		T_DHT11_Byte_Data <<= 1;

		T_DHT11_Byte_Data |= DHT11_Read_Bit();	// 一位一位的读取
	}

	return T_DHT11_Byte_Data;	// 返回读取字节
}
//======================================================================



// 完整的读取DHT11数据
//-----------------------------------------------
// 返回值：		0		DHT11数据读取成功
//				1		结束信号的低电平时长超时
//				2		启动DHT11传输_失败
//				3		校验错误
//==============================================================================
u8 ICACHE_FLASH_ATTR DHT11_Read_Data_Complete(void)
{
	u8 C_delay_time = 0;	// 延时计时

	// 启动DHT11传输_成功
	//------------------------------------------------------------------------
	if(DHT11_Start_Signal_JX() == 0)	// DHT11：输出起始信号－＞接收响应信号
	{
		DHT11_Data_Array[0] = DHT11_Read_Byte();	// 湿度_整数_部分
		DHT11_Data_Array[1] = DHT11_Read_Byte();	// 湿度_小数_部分
		DHT11_Data_Array[2] = DHT11_Read_Byte();	// 温度_整数_部分
		DHT11_Data_Array[3] = DHT11_Read_Byte();	// 温度_小数_部分
		DHT11_Data_Array[4] = DHT11_Read_Byte();	// 校验字节


		// 如果此时是最后一位数据的高电平，则等待它结束
		//-----------------------------------------------------------
		while(GPIO_INPUT_GET(GPIO_ID_PIN(5))==1 && C_delay_time<100)
		{
			os_delay_us(1);		// 1us计时
			C_delay_time++;
		}

		C_delay_time = 0 ;		// 低电平计时开始


		// 结束信号的低电平时长计时
		//-----------------------------------------------------------
		while(GPIO_INPUT_GET(GPIO_ID_PIN(5))==0 && C_delay_time<100)
		{
			os_delay_us(1);		// 1us计时
			C_delay_time++;
		}

		//-----------------------------------------------------------
		if(C_delay_time >= 100)
			return 1;		// 返回1，表示：结束信号的低电平时长超时


		// 数据校验
		//-----------------------------------------------
		if(	DHT11_Data_Array[4] ==
			DHT11_Data_Array[0] + DHT11_Data_Array[1] +
			DHT11_Data_Array[2] + DHT11_Data_Array[3] )
		{

			// 读取DHT11数据结束，ESP8266接管DHT11信号线
			//-----------------------------------------------------------
			//os_delay_us(10);
			//DHT11_Signal_Output(1);	// DHT11信号线输出高(ESP8266驱动)


			// 判断温度是否为 0℃以上
			//----------------------------------------------
			if((DHT11_Data_Array[3]&0x80) == 0)
			{
				DHT11_Data_Array[5] = 1;		// >=0℃
			}
			else
			{
				DHT11_Data_Array[5] = 0;		// <0℃

				DHT11_Data_Array[3] &= 0x7F;	// 更正温度小数部分
			}


			return 0;	// 返回0，表示：温湿度读取成功
		}

		else return 3;		// 返回3，表示：校验错误
	}

	//-----------------------------------------------------
	else return 2;		// 返回2，表示：启动DHT11传输，失败
}
//==============================================================================


// DHT11数据值转成字符串
//======================================================================
void ICACHE_FLASH_ATTR DHT11_NUM_Char(void)
{
	u8 C_char = 0;		// 字符计数

	// 湿度数据字符串
	//…………………………………………………………………………………………
	if(DHT11_Data_Array[0]/100)			// 湿度整数的百位
		DHT11_Data_Char[0][C_char++] = DHT11_Data_Array[0]/100 + 48;

	if((DHT11_Data_Array[0]%100)/10)	// 湿度整数的十位
		DHT11_Data_Char[0][C_char++] = (DHT11_Data_Array[0]%100)/10 + 48;

	// 湿度整数的个位
	//---------------------------------------------------------
	DHT11_Data_Char[0][C_char++] = DHT11_Data_Array[0]%10 + 48;

	DHT11_Data_Char[0][C_char++] = '.';		// 小数点

	// 湿度整数的小数
	//---------------------------------------------------------
	DHT11_Data_Char[0][C_char++] = DHT11_Data_Array[1]%10 + 48;

	DHT11_Data_Char[0][C_char++] = ' ';		// ' '
	DHT11_Data_Char[0][C_char++] = '%';		// '%'
	DHT11_Data_Char[0][C_char++] = 'R';		// 'R'
	DHT11_Data_Char[0][C_char++] = 'H';		// 'H'
	DHT11_Data_Char[0][C_char] 	 =  0 ;		// 添0
	//…………………………………………………………………………………………

	C_char = 0;		// 重置

	// 温度数据字符串
	//…………………………………………………………………………………………
	if(DHT11_Data_Array[5]==0)			// 温度 < 0℃
		DHT11_Data_Char[1][C_char++] = '-';

	if(DHT11_Data_Array[2]/100)			// 湿度整数的百位
		DHT11_Data_Char[1][C_char++] = DHT11_Data_Array[2]/100 + 48;

	if((DHT11_Data_Array[2]%100)/10)	// 湿度整数的十位
		DHT11_Data_Char[1][C_char++] = (DHT11_Data_Array[2]%100)/10 + 48;

	// 湿度整数的个位
	//---------------------------------------------------------
	DHT11_Data_Char[1][C_char++] = DHT11_Data_Array[2]%10 + 48;

	DHT11_Data_Char[1][C_char++] = '.';		// 小数点

	// 湿度整数的小数
	//---------------------------------------------------------
	DHT11_Data_Char[1][C_char++] = DHT11_Data_Array[3]%10 + 48;

	DHT11_Data_Char[1][C_char++] = ' ';		// ' '
	DHT11_Data_Char[1][C_char++] = 'C';		// 'C'
	DHT11_Data_Char[1][C_char]   =  0 ;		// 添0
	//…………………………………………………………………………………………
}
//======================================================================
