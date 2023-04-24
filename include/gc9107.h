#pragma once
#include "stdint.h"

#define TFT_X_OFFSET  0
#define TFT_Y_OFFSET  0

#define TFT_MADCTL    0x36
#define TFT_MAD_MY    0x80
#define TFT_MAD_MX    0x40
#define TFT_MAD_MV    0x20
#define TFT_MAD_ML    0x10
#define TFT_MAD_BGR   0x08
#define TFT_MAD_MH    0x04
#define TFT_MAD_RGB   0x00

#define TFT_INVOFF    0x20
#define TFT_INVON     0x21

#define TFT_DC        7
#define TFT_RES       8
#define TFT_CS        9
#define TFT_MOSI      6
#define TFT_SCK       5
#define TFT_BL        10

#define TFT_SCK_H     digitalWrite(TFT_SCK, 1);
#define TFT_SCK_L     digitalWrite(TFT_SCK, 0);
#define TFT_SDA_H     digitalWrite(TFT_MOSI, 1);
#define TFT_SDA_L     digitalWrite(TFT_MOSI, 0);

#define TFT_RES_H     digitalWrite(TFT_RES, 1);
#define TFT_RES_L     digitalWrite(TFT_RES, 0);
#define TFT_DC_H      digitalWrite(TFT_DC, 1);
#define TFT_DC_L      digitalWrite(TFT_DC, 0);
#define TFT_CS_H      digitalWrite(TFT_CS, 1);
#define TFT_CS_L      digitalWrite(TFT_CS, 0);

#define SPI_FREQUENCY 80000000
#define TFT_SPI_MODE  SPI_MODE0

void gc9107_init(void);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_setRotation(uint8_t r);
void lcd_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void lcd_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void lcd_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
void lcd_PushColors(uint16_t  x,
                    uint16_t  y,
                    uint16_t  width,
                    uint16_t  high,
                    uint16_t *data);
void lcd_PushColors(uint16_t *data, uint32_t len);
void lcd_PushColors_SoftRotation(uint16_t  x,
                                 uint16_t  y,
                                 uint16_t  width,
                                 uint16_t  high,
                                 uint16_t *data,
                                 uint8_t   r);
void lcd_sleep(void );