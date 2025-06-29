
#include "usart.h"
#include "dma.h"
#include "uart_library.h"
#define GPS_BUFFER_LENGTH 300

typedef enum {			// kastomowy enum dla validity
    V, A, Na
} Validity;

typedef enum {			// enum dla długości
    N, S, Nalat
} latitudeDirection;

typedef enum {			// enum dla szerokości
    E, W, Nalon
} longitudeDirection;


typedef struct {		// struktura dla pakitu GPRMC
    char* GPRMC;
    int time;
    Validity val;
    float lat;
    latitudeDirection lat_dir;
    float lon;
    longitudeDirection lon_dir;


} GPRMC;


typedef struct {		// struktura calego komunikatu

	  char* line;		// slużbowa zmienna do przechowywania linijki komunikatu
	  char* codec;		// slużbowa zmienna do odkodowania
	  GPRMC gprmc;		// pakiet GPRMC

	  char* GPVTG;
	  char* GPGGA;
	  char* GPGSA;
	  char* GPGSV;
  } message ;


void parceMessage(message *mes, uint8_t[] );

char getLatDir(message *mes);

char getLonDir(message *mes);

//void check_for_gps_line(uint16_t gps_line_pos, uint8_t rx_buf[],char gps_line_buffer[]);
void check_for_gps_line(message *mes, uint16_t gps_line_pos, uint8_t rx_buf[],char gps_line_buffer[]);
