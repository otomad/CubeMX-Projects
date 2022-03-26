//-----------------------------------------------------------------
// 程序描述:
//     STM32与FPGA的SPI通讯实验
// 作    者: 凌智电子
// 开始日期: 2018-08-04
// 完成日期: 2018-08-04
// 修改日期:
// 当前版本: V1.0
// 历史版本:
//  - V1.0: (2018-08-04)	STM32与FPGA的SPI通讯
// 调试工具: 凌智STM32F429+CycloneIV电子系统设计开发板、LZE_ST_LINK2
// 说    明:
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "stm32f429_winner.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "sdram.h"
#include "spi_communication.h"
#include "string.h"
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------
const int LENGTH = 12;
int main(void) {
	u32 rece_date;
	u8 i, j, buf[16];
	u8 Errors[LENGTH];
	u8 Right[LENGTH];
	HAL_Init();          						// 初始化HAL库
	SystemClock_Config(360, 25, 2, 8); // 设置系统时钟,180Mhz
	SDRAM_Init();                   // 初始化SDRAM
	LCD_Init();                     // 初始化LCD
	SPI_Communication_Init();				// 初始化SPI_Communication
	uart_init(9600);
	POINT_COLOR = BLACK;
	LCD_ShowString(lcddev.width / 2 - 37 * LCD_POINT / 4, 10, 500, LCD_POINT, LCD_POINT, "STM32 And FPGA SPI Communication Test");
	printf("STM32 And FPGA SPI Communication Test\r\n");

	while (1) {
		memset(Errors, 0, LENGTH);	// 数组清零
		memset(Right, 0, LENGTH);	// 数组清零
		for (i = 0; i < 100; i++) {
			for (j = 0; j < LENGTH; j++) {
				SPI_Communication_Send_Cmd_Data(j, i);			// 发送数据
			}
			for (j = 0; j < LENGTH; j++) {
				rece_date = SPI_Communication_Rece_Cmd_Data(j);	// 读取数据
				if (rece_date == i) {
					Right[j]++;
					POINT_COLOR = BLUE;
				} else {
					Errors[j]++;
					POINT_COLOR = RED;
				}
				printf("CH%1d -> DATA:%3d  Errors:%3d  Right:%3d\r\n", j, rece_date, Errors[j], Right[j]);
				sprintf((char *)buf, "CH%1d -> DATA:%3d  Errors:%3d  Right:%3d", j, rece_date, Errors[j], Right[j]);
				LCD_ShowString(lcddev.width / 2 - 38 * LCD_POINT / 4, lcddev.height / 2 - 4 * LCD_POINT + j * LCD_POINT, 500, LCD_POINT, LCD_POINT, (char *)buf);
				delay_ms(1);
			}
			delay_ms(500);
		}
	}
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
