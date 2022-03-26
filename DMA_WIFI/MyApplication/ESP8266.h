#ifndef _ESP8266_H
#define _ESP8266_H

#include "stdint.h"

void ESP8266_SendCmd(char *pbuf);
void ESP8266_ModeInit(void);
void send(const char *str);
void sendAt(const char *str);
void dueIPD(uint8_t BUFFER_SIZE, uint8_t rx_len, uint8_t *rx_buffer, char *buffer);
extern int Esp8266HasInited;

#endif





