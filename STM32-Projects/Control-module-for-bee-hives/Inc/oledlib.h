
#ifndef OLED_LIB_H

#define OLED_LIB_H


void beginOLED(SPI_HandleTypeDef *hspi);

void clearPage(SPI_HandleTypeDef *hspi, uint8_t page);

void fillPage(SPI_HandleTypeDef *hspi, uint8_t page);

void frameTrans(SPI_HandleTypeDef *hspi);

void clearGDDRAM(SPI_HandleTypeDef *hspi);

void setPixelBuffer(uint8_t x, uint8_t y);

void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

void drawCircle(uint8_t x, uint8_t y, uint8_t r);

void printChar(uint8_t x, uint8_t y, uint8_t c);

void printString(uint8_t x, uint8_t y, char val[]);

void printInt(uint8_t x, uint8_t y, int val);

void priLinHex(uint8_t x, uint8_t y, uint64_t val);

void clearBuffer();

#endif // OLED_LIB_H


