/*
*******************************************************************************
  * Autorzy:	Arseniy Sialitski
  *		Przemysław Ziencik
  * Ostatnia aktualizacja: 10.06.2025
  * Opis: Biblioteka z funkcjami do obsługi wyświetlacza OLED128x64px
  *
******************************************************************************
*/

#include "main.h"
#include "stdlib.h"

#include "ASCII.h"
#include "buffers.h"

#include <string.h>
#include <stdio.h>

#include <math.h>

//************** change to your pins *******************

#define CS_PIN CS_Pin			// GPIO pin CS
#define DC_PIN DC_Pin			// GPIO pin DC

#define CS_PIN_PORT CS_GPIO_Port			// GPIO port of CS pin
#define DC_PIN_PORT DC_GPIO_Port			// GPIO port of DC pin

//


void beginOLED(SPI_HandleTypeDef *hspi){			// transmit configuration buffer
													//to start display functioning
	  transmit_buffer[0] = 0x00;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);

	  // config
	  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 0);		// DC
	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);		// CS

	  for(int i = 0; i < 19 ; i++){	// 55 65
		  transmit_buffer[0] = config_buffer[i];
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  }

	  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 1);		// DC
	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 1);		// CS


	  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 0);		// DC
	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);		// CS

	  transmit_buffer[0] = 0xaf;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  HAL_Delay(10);

}

void clearPage(SPI_HandleTypeDef *hspi, uint8_t page){			// fill display "page" with zeros
	 HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 0);			// DC
	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);		// CS
	  transmit_buffer[0] = 0x10;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  transmit_buffer[0] = 0x00;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  transmit_buffer[0] = page;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);

	  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 1);		// DC
	  for(int i =0; i < 128; i++){
		  transmit_buffer[0] = 0x00;
		  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  }
	  	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 1);
	  	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);

}


void fillPage(SPI_HandleTypeDef *hspi, uint8_t page){			// fill display "page" with ones
	 HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 0);			// DC
	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);		// CS
	  transmit_buffer[0] = 0x10;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  transmit_buffer[0] = 0x00;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  transmit_buffer[0] = page;
	  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);

	  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 1);		// DC
	  for(int i =0; i < 128; i++){
		  transmit_buffer[0] = 0xff;
		  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
	  }

}

void frameTrans(SPI_HandleTypeDef *hspi){					// transmit updated buffer
			  for(int i =0; i < 8; i++){

				  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 0);		// DC
				  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);		// CS
				  transmit_buffer[0] = 0x10;
				  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
				  transmit_buffer[0] = 0x00;
				  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
				  transmit_buffer[0] = 0xb0 + i;
				  HAL_SPI_Transmit(hspi, transmit_buffer, 1, 40);
				  HAL_GPIO_WritePin(DC_PIN_PORT, DC_PIN, 1);		// DC

			  HAL_SPI_Transmit(hspi, buffer[i], 128, 40);

		  }

		  	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 1);
		  	  HAL_GPIO_WritePin(CS_PIN_PORT, CS_PIN, 0);

}

void clearGDDRAM(SPI_HandleTypeDef *hspi){		// clear display GDDRAM
												// set all "pages" to zero
		  			  clearPage(hspi,0xb0);
		  			  clearPage(hspi,0xb1);
		  			  clearPage(hspi,0xb2);
		  			  clearPage(hspi,0xb3);
		  			  clearPage(hspi,0xb4);
		  			  clearPage(hspi,0xb5);
		  			  clearPage(hspi,0xb6);
		  			  clearPage(hspi,0xb7);
		  		}


void setPixelBuffer(uint8_t x, uint8_t y){			// set selected pixel to HIGH state

	if(x <= 127){
	if(y <= 7)
		buffer[0][x]  ^= (0x01 << y);		// can change |= to ^=
	else if(y <= 15)
		buffer[1][x] ^= (0x01 << y%8);
	else if(y <= 23)
			buffer[2][x] ^= (0x01 << y%8);
	else if(y <= 31)
			buffer[3][x] ^= (0x01 << y%8);
	else if(y <= 39)
			buffer[4][x] ^= (0x01 << y%8);
	else if(y <= 47)
			buffer[5][x] ^= (0x01 << y%8);
	else if(y <= 55)
			buffer[6][x] ^= (0x01 << y%8);
	else if(y <= 63)
			buffer[7][x] ^= (0x01 << y%8);

	}


}


void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){	// draw line from one point to another
	int steep = 0;												// V3 of line drawing algorithm
	if(abs(x1-x2) < abs(y1-y2)){
		x1 = x1 ^ y1;
		y1 = x1 ^ y1;
		x1 = x1 ^ y1;

		x2 = x2 ^ y2;
		y2 = x2 ^ y2;
		x2 = x2 ^ y2;
		steep++;
	}

	if(x1 > x2){
		x1 = x1 ^ x2;
		x2 = x1 ^ x2;
		x1 = x1 ^ x2;

		y1 = y1 ^ y2;
		y2 = y1 ^ y2;
		y1 = y1 ^ y2;
	}

	for(int x = x1; x<= x2; x++){
		double t =  (x-x1)/(double)(x2-x1);

		int y = y1 * (1. - t) + y2*t;
		if(steep){
			setPixelBuffer(y, x);

		}else{
			setPixelBuffer(x, y);

		}

	}
}

void drawCircle(uint8_t x, uint8_t y, uint8_t r){

	for(double t = 0.0; t<6.3;t += 0.01 ){
		  int x_temp = x + r*cos(t);
		  int y_temp = y + r*sin(t);
		  setPixelBuffer(x_temp, y_temp);
	  }
	}



void printChar(uint8_t x, uint8_t y, uint8_t c){		// print char in selected point
	if((c >=32)&&(c <= 127)){											// char has to an int code of ASCII
	for(int i = 0; i < 8; i++){							// ex. to print "A" parameter c must be 65
		for(int j = 0; j <8; j++){
				if((ASCII[c-32][i] << j)& 0b10000000 ){
					setPixelBuffer(x+j, y+i);
				}
	}
	}
	}
}

void printString(uint8_t x, uint8_t y, char val[]){		// print string in selected point
	for(int i = 0; i < strlen(val); i++){
		printChar(x+i*6,y,(int)val[i]);
	}
}

void printInt(uint8_t x, uint8_t y, int val){
	char str[10];
	sprintf(str,"%d",val);
	printString(x, y, str);
}

void priLinHex(uint8_t x, uint8_t y, uint64_t val){
	for(int i = 0; i < 64; i++){
		if((val >> i)&0b00000001){
			setPixelBuffer(x+i, y);
		}
	}
}



void clearBuffer(){
	for(int j = 0; j < 8; j++){
	for(int i = 0; i < 128; i++){
		buffer[j][i] = 0;
	}
}
}
