/*
*******************************************************************************
  * Autorzy:	Arseniy Sialitski
  *		Przemysław Ziencik
  * Ostatnia aktualizacja: 10.06.2025
  * Opis: Biblioteka z funkcjami do obslugi gps
  *
  *
******************************************************************************
*/
#include "main.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "gpsparce.h"

#include "usart.h"
#include "dma.h"
#include "uart_library.h"

#define GPS_BUFFER_LENGTH 300

void parceMessage(message *mes, uint8_t rx_buffer[]){
	char* restr;
	char rest[300] = {0};
//	  if(HAL_UART_Receive(&huart2, rx_buff, 300, 1500) == HAL_OK){
		  //if(1){

	    strcpy(rest,(char*)rx_buffer);
			  restr = rest;

			  while(strlen(restr)){

			  mes->line = strtok_r(restr, "$", &restr);
			  mes->codec = strtok_r(mes->line, ",", &(mes->line));


			 if(strcmp(mes->codec,"GPRMC")==0){
				 mes->gprmc.GPRMC = mes->line;
				 //m1.gprms.GPRMC[strlen(m1.gprms.GPRMC)-1] = '\0';

				 mes->codec = strtok_r(mes->gprmc.GPRMC, ",", &mes->gprmc.GPRMC);			// latitude
				 //m1.codec[strlen(m1.codec)] = '\0';

				 mes->gprmc.time = strtol(mes->codec,NULL,10);			// UTC time


				 if(strcmp(strtok_r(mes->gprmc.GPRMC ,",",&mes->gprmc.GPRMC),"V")==0){	// validity
					 mes->gprmc.val = V;
				 }else{
					 mes->gprmc.val = A;
				 }

				 mes->codec = strtok_r(mes->gprmc.GPRMC ,",",&mes->gprmc.GPRMC);			// latitude
				 mes->codec[strlen(mes->codec)] = '\0';
				 mes->gprmc.lat = atof(mes->codec);

				 mes->gprmc.lat =  floor(mes->gprmc.lat/100.0) + (mes->gprmc.lat/100.0 - floor(mes->gprmc.lat/100.0))/0.6 ; // 2052.852295

				 if(strcmp(strtok_r(mes->gprmc.GPRMC ,",",&mes->gprmc.GPRMC),"N")==0){	// latitude direction
					 mes->gprmc.lat_dir = N;
				 }else{
					 mes->gprmc.lat_dir = S;
				 }

				 mes->codec = strtok_r(mes->gprmc.GPRMC ,",",&mes->gprmc.GPRMC);			// longitude
				 mes->codec[strlen(mes->codec)] = '\0';
				 mes->gprmc.lon = atof(mes->codec);

				 mes->gprmc.lon =  floor(mes->gprmc.lon/100.0) + (mes->gprmc.lon/100.0 - floor(mes->gprmc.lon/100.0))/0.6 ; // 2052.852295


				 if(strcmp(strtok_r(mes->gprmc.GPRMC ,",",&mes->gprmc.GPRMC),"W")==0){	// longitude direction
					 mes->gprmc.lon_dir = W;
				 }else{
					 mes->gprmc.lon_dir = E;
				 }
				 /*
				 add speed in knots, track true and date
				 */
/////////////////// in progress


			 } else if(strcmp(mes->codec,"GPVTG")==0){			// data good, speed relative data
				 mes->GPVTG = mes->line;
				 mes->GPVTG[strlen(mes->GPVTG)-1] = '\0';

			 }
			   else if(strcmp(mes->codec,"GPGGA")==0){			// GNSS receiver data
			 	 mes->GPGGA = mes->line;
			 	 mes->GPGGA[strlen(mes->GPGGA)-1] = '\0';


	  	    } else if(strcmp(mes->codec,"GPGSA")==0){
	  	    	//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	  	    	mes->GPGSA = mes->line;
	  	    	mes->GPGSA[strlen(mes->GPGSA)-1] = '\0';


		   } else if(strcmp(mes->codec,"GPGSV")==0){
			   //HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			   mes->GPGSV = mes->line;
			   mes->GPGSV[strlen(mes->GPGSV)-1] = '\0';

		   }
			  }

//}
}

char getLatDir(message *mes){
	switch(mes->gprmc.lat_dir) {
	case N:
		return 'N';
	break;
	case S:
		return 'S';
	default:
		return '-';
	}
}

char getLonDir(message *mes){
	switch(mes->gprmc.lon_dir) {
	case E:
		return 'E';
	break;
	case W:
		return 'W';
	default:
		return '-';
	}
}
/*
void check_for_gps_line(message *m1, uint16_t gps_line_pos, uint8_t rx_buf[],char gps_line_buffer[]){
    static uint16_t old_pos = 0;
    uint16_t pos = GPS_BUFFER_LENGTH - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    if (pos != old_pos){
        for(uint16_t i = old_pos; i != pos; i = (i + 1) % GPS_BUFFER_LENGTH){
            char c = rx_buf[i];
            gps_line_buffer[gps_line_pos++] = c;
            if (c == '\n') {
                gps_line_buffer[gps_line_pos] = '\0';
                // np. przekazanie do funkcji parseMessage
                parceMessage(&m1, gps_line_buffer);
                gps_line_pos = 0;
            }
            if(gps_line_pos >= sizeof(gps_line_buffer)){
                gps_line_pos = 0; // overflow protection
            }
        }
        old_pos = pos;
    }
}
*/


void check_for_gps_line(message *mes, uint16_t gps_line_pos, uint8_t rx_buf[],char gps_line_buffer[]){
    static uint16_t old_pos = 0;
    uint16_t pos = GPS_BUFFER_LENGTH - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    if (pos != old_pos){
        for(uint16_t i = old_pos; i != pos; i = (i + 1) % GPS_BUFFER_LENGTH){
            char c = rx_buf[i];
            gps_line_buffer[gps_line_pos++] = c;
            if (c == '\n') {
                gps_line_buffer[gps_line_pos] = '\0';
                // np. przekazanie do funkcji parseMessage
                parceMessage(&mes, gps_line_buffer);
                gps_line_pos = 0;
            }
            if(gps_line_pos >= sizeof(gps_line_buffer)){
                gps_line_pos = 0; // overflow protection
            }
        }
        old_pos = pos;
    }
}
