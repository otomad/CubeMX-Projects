/**
单片机最高可设置的波特率为9600，所以先通过下载器把ESP8266改为9600波特率(AT+CIOBAUD=9600)；
单片机可设置的波特率通过串口中断初始化（波特率）设置

**/
//#include "public.h"
//#include "UART.h"
#include "ESP8266.h"
#include "main.h"
#include "usart.h"

#include <string.h>
#include <stdio.h>

int Esp8266HasInited = 0;

/**
  * @brief  实现发送指令，并换新行
  * @param  *cmd
  * @retval None
  **/
/*void ESP8266_SendCmd(uint8_t *pbuf)
{
	while(*pbuf!='\n') //遇到空格跳出循环	
	{
		UART_SendByte(*pbuf);
		HAL_Delay(1);
		pbuf++;	
	}
	HAL_Delay(1);
	UART_SendByte('\n');//换行
}*/

/**
 * @brief  ESP8266-WIFI模块工作模式初始化AP服务端
 * @retval None
 */
void ESP8266_ModeInit(void) {
	const static int wait = 500;
	
	sendAt("AT+CWMODE=2\r\n");//设置路由器模式 1 staTIon模式 2 AP点 路由器模式 3 station+AP混合模式
	HAL_Delay(wait);
	
	sendAt("AT+CWSAP=\"ESP8266\",\"12345678\",11,3\r\n"); //设置WIFI热点名及密码
	HAL_Delay(wait);
	
	sendAt("AT+CIPAP=\"192.168.4.1\"\r\n");//重新启动wifi模块
	HAL_Delay(wait);

//	ESP8266_SendCmd("AT+RST");//重新启动wifi模块
	sendAt("AT+CIPMUX=1\r\n");	//开启多连接模式，允许多个各客户端接入
	HAL_Delay(wait);

	sendAt("AT+CIPSERVER=1,8080\r\n");	//启动TCP/IP 端口为8080 实现基于网络控制	
	HAL_Delay(wait);
	
	Esp8266HasInited = 1;
}

/**
 * @brief  这个函数没有开头的计算长度，用于发送模块内部命令
 * @retval None
 */
void sendAt(const char *str) {
	HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 200);
}

/**
 * @brief  这个函数用于ESP8266使用AT指令发送数据
 * @retval None
 */
void send(const char *str) {
	if (!isSerialPort) {
		char AT[19];
		uint16_t len = strlen(str);
		sprintf(AT, "AT+CIPSEND=0,%d\r\n", len);
		sendAt(AT);
		HAL_Delay(10);
	}
	/*char strWithPrefix[len];
	strWithPrefix[0] = '!';
	strcat(strWithPrefix, str);
	sendAt(strWithPrefix);*/
	sendAt(str);
}

/**
 * @brief  这个函数用于对ESP8266接收数据时产生AT指令进行过滤，处理成我们需要的内容并反馈。
 * 这个函数仅在DMA接收数据函数中使用。
 * @param  {uint8_t}   BUFFER_SIZE - 缓存区接收的拟定最大长度。
 * @param  {uint8_t}   rx_len      - 缓存区接收数据位长度。
 * @param  {uint8_t *} rx_buffer   - 缓存区接收数据位的内容数据。
 * @param  {char *}    buffer      - 待寄存的字符串数组。
 * @retval None
 */
void dueIPD(uint8_t BUFFER_SIZE, uint8_t rx_len, uint8_t *rx_buffer, char *buffer) {
		//printf("rx_len=%d\r\n", rx_len); // 打印接收长度
		uint8_t len = 0;
		//#if !test
		if (!isSerialPort)
		{ // uint_8转char[]
			/*char IPD[] = "+IPD";
			for (int j = 0; j < strlen(IPD); j++)
				if (IPD[j] != rx_buffer[j] && !isSerialPort)
					goto clear;
			uint8_t i;
			short isMsg = 0; // 垃圾STM32既没有bool也没有bit
			for (i = 0; i < rx_len && i < BUFFER_SIZE; i++) {
				const char ch = rx_buffer[i];
				if (ch == ':' && !isMsg && !isSerialPort)
					isMsg = 1;
				else if (isMsg || isSerialPort)
					buffer[len++] = ch;
			}
			buffer[len] = '\0';
			if (len == 0 || !isMsg) goto clear;*/
			short isMsg = 0; // 垃圾STM32既没有bool也没有bit
			const static char IPD[] = "+IPD";
			for (int i = 0; i < rx_len && i < BUFFER_SIZE; i++)
				switch (isMsg) {
					case 0:
						for (int j = 0; j < strlen(IPD); j++)
							if (IPD[j] != rx_buffer[i + j])
								break;
						/*if (
							rx_buffer[i] == '+' &&
							rx_buffer[i + 1] == 'I' &&
							rx_buffer[i + 2] == 'P' &&
							rx_buffer[i + 3] == 'D'
						)*/
						isMsg++;
						break;
					case 1:
						if (
							rx_buffer[i] == ':'
						) isMsg++;
						break;
					case 2:
						//if (rx_buffer[i] == '\r' || rx_buffer[i] == '\n') break;
						buffer[len++] = rx_buffer[i];
					default:
						break;
				}
		}
		//#else
		else {
			for (int i = 0; i < rx_len && i < BUFFER_SIZE; i++)
				buffer[len++] = rx_buffer[i];
		}
		buffer[len] = '\0';
		//#endif
		if (len) { //不弄个花括号会奇葩报错，明明不会有任何问题。
			char sayHello[len + 10];
			sprintf(sayHello, "Hello, %s\r\n", buffer);
			//char sayHello[210];
			//sprintf(sayHello, "%c,%d,Hello, %s\r\n", isMsg?'Y':'N',len,buffer);
			send(sayHello);
			//HAL_UART_Transmit(&huart1, rx_buffer, rx_len, 200); // 接收数据打印出来
		}
}
