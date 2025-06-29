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

#ifndef SYIRDEVICE_ACC_H
#define SYIRDEVICE_ACC_H

//ADXL345 Register Addresses
#define	ACC_REG_DEVID		    0x00	//Device ID Register
#define ACC_REG_THRESH_TAP	    0x1D	//Tap Threshold
#define	ACC_REG_OFSX		    0x1E	//X-axis offset
#define	ACC_REG_OFSY		    0x1F	//Y-axis offset
#define	ACC_REG_OFSZ		    0x20	//Z-axis offset
#define	ACC_REG_DURATION	    0x21	//Tap Duration
#define	ACC_REG_LATENT		    0x22	//Tap latency
#define	ACC_REG_WINDOW		    0x23	//Tap window
#define	ACC_REG_THRESH_ACT	    0x24	//Activity Threshold
#define	ACC_REG_THRESH_INACT	0x25	//Inactivity Threshold
#define	ACC_REG_TIME_INACT	    0x26	//Inactivity Time
#define	ACC_REG_ACT_INACT_CTL	0x27	//Axis enable control for activity and inactivity detection
#define	ACC_REG_THRESH_FF	    0x28	//free-fall threshold
#define	ACC_REG_TIME_FF		    0x29	//Free-Fall Time
#define	ACC_REG_TAP_AXES	    0x2A	//Axis control for tap/double tap
#define ACC_REG_ACT_TAP_STATUS	0x2B	//Source of tap/double tap
#define	ACC_REG_BW_RATE		    0x2C	//Data rate and power mode control
#define ACC_REG_POWER_CTL	    0x2D	//Power Control Register
#define	ACC_REG_INT_ENABLE	    0x2E	//Interrupt Enable Control
#define	ACC_REG_INT_MAP		    0x2F	//Interrupt Mapping Control
#define	ACC_REG_INT_SOURCE	    0x30	//Source of interrupts
#define	ACC_REG_DATA_FORMAT	    0x31	//Data format control
#define ACC_REG_DATAX0		    0x32	//X-Axis Data 0
#define ACC_REG_DATAX1		    0x33	//X-Axis Data 1
#define ACC_REG_DATAY0		    0x34	//Y-Axis Data 0
#define ACC_REG_DATAY1		    0x35	//Y-Axis Data 1
#define ACC_REG_DATAZ0		    0x36	//Z-Axis Data 0
#define ACC_REG_DATAZ1		    0x37	//Z-Axis Data 1
#define	ACC_REG_FIFO_CTL	    0x38	//FIFO control
#define	ACC_REG_FIFO_STATUS	    0x39	//FIFO status

    extern void     syirDevice_INIT_acc(void);
    extern int8_t   syirDevice_READ_accReg(uint8_t reg, uint8_t *val);
    extern int8_t   syirDevice_WRITE_accReg(uint8_t reg, uint8_t val);
    extern uint8_t  syirDevice_XYZ_acc(int16_t *x, int16_t *y, int16_t *z);
    extern uint8_t  syirDevice_EVENT_acc(uint8_t *ev);
    extern uint8_t syirDevice_STATUS_acc(void);

#endif