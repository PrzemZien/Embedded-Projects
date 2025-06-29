/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/* Autorzy:  Arkadiusz Luczyk; Jakub Jasinski; Marek Niewi?ski               */
/* Email  :  arkadiusz.luczyk@pw.edu.pl                                      */
/*           jakub.jasinski@pw.edu.pl                                        */
/*           marek.niewinski@pw.edu.pl                                       */
/*                                                                           */
/*     Do wy??cznego u?ytku w ramach zaj?? SYIR                              */
/*     Do wylacznego uzytku w ramach zajec SYIR                              */
/*                                                                           */
/* ************************************************************************* */

#include <xc.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <syirDevice.h>

typedef struct syirDevice_StatusACC {
    volatile uint8_t    trigger;
    volatile uint8_t    state;
             uint8_t    FIFO_STATUS;
             int16_t    x;
             int16_t    y;
             int16_t    z;
             uint8_t    fifo_ctl;
             uint8_t    fifo_status;
             uint8_t    reg;
             uint8_t    val;
             uint8_t *  ptr;    
} syirDevice_StatusACC_t;

syirDevice_StatusACC_t statusACC;

#define SPI_CS_1 PORTC.OUT |= 0b00010000;
#define SPI_CS_0 PORTC.OUT &= 0b11101111;

void syirDevice_INIT_acc(void) {

    statusACC.state = 0;
    statusACC.x = 0;
    statusACC.y = 0;
    statusACC.z = 0;
    statusACC.reg = 0xff;
    statusACC.trigger = 0;
    statusACC.fifo_ctl = 0;
    statusACC.fifo_status = 1;    
    { 
     // ACC power on
        PORTC.DIR       |= (1 << 1);
        PORTC.OUT       |= (1 << 1);  
        PORTC.DIR       |= (1 << 4); // SS - out
        SPI_CS_1;

        PORTC.DIR       |= (1 << 5); // MOSI - out
        PORTC.DIR       |= (1 << 7); // SCK - out

        SPIC.CTRL       = 0;
        SPIC.INTCTRL    |= (1 << 0);
        SPIC.CTRL       |= (0 << 0); //Preskaler DIV_8
        SPIC.CTRL       |= (3 << 2); // MODE Falling, setup Rising, sample
        
        SPIC.CTRL       |= (1 << 4); // MASTRE mode
        SPIC.CTRL       |= (1 << 6); // SPIC enable
        
        // PORTD.0 - interrupt setting
        PORTD.DIR     &= ~(1 << 0);
        PORTD.PIN0CTRL |= (1 << 0);
        PORTD.INT0MASK |= (1 << 0);
        PORTD.INTCTRL  |= (1 << 0);
    }
}

int8_t syirDevice_READ_accReg(uint8_t reg, uint8_t *ptr) {

    switch (statusACC.state) {
        case 0: {
            statusACC.state = 1;
            statusACC.reg = reg;
            statusACC.ptr = ptr;
            cli();
            {
                SPI_CS_0;
                SPIC.DATA = (reg | (1 << 7)) & ~(1 << 6);
            }     
            sei();
            return 0;
        }
        case 1: return 0;
        case 2: {
            statusACC.state = 3;
            cli();
            {
                statusACC.val   = SPIC.DATA;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }
        case 3 : return 0;
        case 4: {
            int8_t val;
            cli();
            {
                val = SPIC.DATA;
                SPI_CS_1;
            }
            sei();
            statusACC.state = 0;
            statusACC.val   = val;
            *statusACC.ptr   = val;
            return 1;
        }
        default:
            return 0;
    }

}

int8_t syirDevice_WRITE_accReg(uint8_t reg, uint8_t val) {

    switch (statusACC.state) {
        case 0: {
            statusACC.state = 5;
            statusACC.reg = reg;
            statusACC.val = val;
            cli();
            {
                SPI_CS_0;
                SPIC.DATA = (reg | (0 << 7)) & ~(1 << 6);
            }     
            sei();  
            return 0;
        }
        case 5: return 0;
        case 6: {
            statusACC.state = 7;
            cli();
            {
                SPIC.DATA = statusACC.val;
            }
            sei();
            return 0;    
        }
        case 7 : return 0;
        case 8: {
            int8_t val;
            cli();
            {
                val = SPIC.DATA;
                SPI_CS_1;
            }
            sei();
            statusACC.state = 0;
            return 1;
        }
        default:
            return 0;
    }

}

uint8_t syirDevice_EVENT_acc(uint8_t *ev) 
{
    if (*ev != statusACC.trigger) {
        *ev = statusACC.trigger;
        return 1;
    }
    return 0;
}

uint8_t syirDevice_STATUS_acc(void)
{
    return statusACC.fifo_status;
}

uint8_t syirDevice_XYZ_acc(int16_t *x, int16_t *y, int16_t *z) {

    switch (statusACC.state) {
        case 0: {
            statusACC.state = 10;
            cli();
            {
                SPI_CS_0;
                SPIC.DATA = (ACC_REG_DATAX0 | (1 << 7)) | (1 << 6); // Read multiple bytes
            }     
            sei();  
            return 0;
        }
        
        // Neglect 1st byte
        case 10: return 0;
        case 11: {
            statusACC.state++;
            cli();
            {
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }

        // Read X
        case 12 : return 0;
        case 13: {
            statusACC.state++;
            cli();
            {
                statusACC.x  = SPIC.DATA;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }
        case 14 : return 0;
        case 15: {
            statusACC.state++;
            cli();
            {
                statusACC.x  |= SPIC.DATA << 8;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }

        // Read Y
        case 16 : return 0;
        case 17: {
            statusACC.state++;
            cli();
            {
                statusACC.y  = SPIC.DATA;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }
        case 18 : return 0;
        case 19: {
            statusACC.state++;
            cli();
            {
                statusACC.y  |= SPIC.DATA << 8;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }

        // Read Z
        case 20 : return 0;
        case 21: {
            statusACC.state++;
            cli();
            {
                statusACC.z  = SPIC.DATA;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }
        case 22 : return 0;
        case 23: {
            statusACC.state++;
            cli();
            {
                statusACC.z  |= SPIC.DATA << 8;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }
        case 24 : return 0;
        case 25: {
            statusACC.state++;
            cli();
            {
                statusACC.fifo_ctl  = SPIC.DATA;
                SPIC.DATA = 0;
            }
            sei();
            return 0;    
        }
        case 26 : return 0;
        case 27: {
            cli();
            {
                statusACC.fifo_status  = SPIC.DATA;
                SPI_CS_1;
            }
            sei();
            *x = statusACC.x; 
            *y = statusACC.y; 
            *z = statusACC.z; 
            statusACC.state = 0;
            
            return 1;
        }

        default:
            return 0;
    }

}


ISR(SPIC_INT_vect)
{
    statusACC.state++;
}

ISR(PORTD_INT0_vect)
{
    statusACC.trigger++;   
}