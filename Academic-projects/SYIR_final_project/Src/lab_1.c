/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/*                                                                           */
/* ************************************************************************* */

#include <syirDevice.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// SYIR device clock frequency: 32 MHz
#define SYIRDEVICE_CLOCK_SOURCE     SYIRDEVICE_CLOCK_INTERNAL_32MHz
#define SYIRDEVICE_CLOCK_DIVISION   SYIRDEVICE_CLOCK_DIV_1

// SYIR system clock: ~26 Hz
#define SYSTEM_CLOCK_PERIOD         1200
#define SYSTEM_CLOCK_DIVISION       SYSTEM_CLOCK_DIV_1024

// UART log settings: 57600 bound rate
#define UARTLOG_BSCALE              -6
#define UARTLOG_BSEL                2158
#define UARTLOG_CLOCK_DIVISION      0

// nasze deifne
#define TEMP_SIZE 100

void swap(float* xp, float* yp){
    float temp = *xp; 
    *xp = *yp;
    *yp = temp;
}

void bubbleSort(float arr[], int n){
    int i, j;
    uint8_t swapped;
    for(i = 0; i < n -1; i++){
        swapped = 0;
        for(j = 0; j < n - i -1; j++){
            if(arr[j] > arr[j+1]){
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }
        if(!swapped){break;}
    }
}

float median_filter(float array_of_values[], float parameter){
    uint8_t iteration = 0;
    float medianMCU;
    array_of_values[iteration] = parameter; 
    uint8_t flag = 0; 
    if(iteration == TEMP_SIZE){
        iteration = 0;
    }else{
        iteration++;
    }
    if(!flag){
        for(int i = 0; i < TEMP_SIZE; i++){
            if(array_of_values[i] == 0){
                flag &= 0;
           } 
            else{
                flag++;
                }
        }
        }
    medianMCU = (array_of_values[(TEMP_SIZE/2)-1] + array_of_values[TEMP_SIZE/2])/2;
    return medianMCU;
}



int main(void) {	
    
    char        log_buf [1024];
    uint16_t    log_len;
    
    uint16_t    cpuM;
    //float       cpuT = 0.1;
    // ------- lab2 ST -------
    uint8_t     qid[8];
    uint8_t     qid_status; 

    uint16_t    vddM;
    float       vddV = 0.1;
    uint8_t     vddF2;
    uint8_t     vddF;
    uint8_t     vddF1;

    systemClock_t qid_ticks;

    syirDeviceTIMEticks_t rtcTICK;
    
    // ------- lab2 END-------
    
    
    // nasz kod
    
    uint16_t external_measure_temp;
    float external_temp = 0.1;
    
    // Zadanie 4 
    float cpuM_internal_temp[TEMP_SIZE] = {0};
    float external_temp_ar[TEMP_SIZE] = {0};
    float voltage_measure[TEMP_SIZE] = {0};
    float cpuT = 0;
    uint8_t liczn = 0;                          // licznik lol
    uint8_t flaga_pelna = 0;                    // flaga probki nezerowe
    float medianMCU = 0.0;
    // deklaracje_filtr
    float medianMCU_int = 0.0f;
    float medianMCU_ext = 0.0f;
    float median_voltage = 0.0f;
    //
    //---------------Laboratorium 3 -----------
    float acc_x_buf[TEMP_SIZE] = {0.0f};
    float acc_y_buf[TEMP_SIZE] = {0.0f};
    float acc_z_buf[TEMP_SIZE] = {0.0f};
    uint8_t acc_index = 0;
    uint8_t         acc_reg[5];
    int16_t         acc_x, acc_y, acc_z;
    
    //---------------Laboratorium 3 -----------
    
    
    systemClock_t led_ticks;
    systemClock_t log_ticks;
    
    syirDevice_OFF_ports();

    if (syirDevice_SET_deviceClock(SYIRDEVICE_CLOCK_SOURCE, SYIRDEVICE_CLOCK_DIVISION) < 0) 
    {
        syirDevice_ON_led(GREEN);
        while (1);
    }
    
    if (syirDevice_SET_systemClock(SYSTEM_CLOCK_PERIOD, SYSTEM_CLOCK_DIVISION) < 0) 
    {
        syirDevice_ON_led(ORANGE);
        while (1);
    }

    /* 57600 baudrate */
    if (!syirDevice_SET_log(UARTLOG_BSCALE, UARTLOG_BSEL, UARTLOG_CLOCK_DIVISION)) 
    {
        syirDevice_ON_led(GREEN);
        syirDevice_ON_led(ORANGE);
        while (1);
    }

    syirDevice_INIT_internalTemperature(0);
    // nasz kod 
    syirDevice_INIT_internalTemperature(1);
    //
    syirDevice_INIT_log();
    //---------------Laboratorium 3 -----------
    syirDevice_INIT_acc();
    //---------------Laboratorium 3 -----------
    // ------- LAB 2 ST -----
    syirDevice_INIT_vdd(0, &vddF2);
    syirDevice_INIT_sleep(SLEEP_POWER_DOWN);   //////////////////////////// SLEEEEEEEEEEEEEP MODE
    syirDevice_INIT_time(TIME_TICK_MILI_SECONDS);
    syirDevice_PERIOD_time(200);
    // ------- LAB 2 EMD -----
        uint8_t ACC_TOG = 0;
    //---------------Laboratorium 3 -----------
    while (!syirDevice_WRITE_accReg(ACC_REG_INT_ENABLE, (0 << 0)));         // INTERRUPT mode - none
    while (!syirDevice_WRITE_accReg(ACC_REG_DATA_FORMAT,(0 << 0)));         // Data range: +/-2g
    while (!syirDevice_WRITE_accReg(ACC_REG_FIFO_CTL, (0b11 << 6) + 16 ));  // FIFO mode - TRIGGER 16 samples
    while (!syirDevice_WRITE_accReg(ACC_REG_BW_RATE,    (0b1011 << 0)));    // Measure rate: 200 Hz
    while (!syirDevice_WRITE_accReg(ACC_REG_INT_ENABLE, (1 << 6)));         // INTERRUPT mode - WATERRUN
    
    // zadanie 4 
    while (!syirDevice_WRITE_accReg(ACC_REG_THRESH_TAP, 16));
    while (!syirDevice_WRITE_accReg(ACC_REG_DURATION,    8));
    while (!syirDevice_WRITE_accReg(ACC_REG_TAP_AXES,  (0b100 << 0))); 
    //while (!syirDevice_WRITE_accReg(ACC_REG_INT_ENABLE, (1 << 6)));         // INTERRUPT mode - Single TAP
    //while (!syirDevice_WRITE_accReg(ACC_REG_INT_MAP,  (1 << 6)));         // Start measure   
    while (!syirDevice_WRITE_accReg(ACC_REG_POWER_CTL,  (1 << 3)));         // Start measure
    // zadanie 4 
    
    while (!syirDevice_READ_accReg(ACC_REG_DEVID,       &acc_reg[0]));
    while (!syirDevice_READ_accReg(ACC_REG_BW_RATE,     &acc_reg[1]));
    while (!syirDevice_READ_accReg(ACC_REG_FIFO_CTL,    &acc_reg[2]));
    while (!syirDevice_READ_accReg(ACC_REG_POWER_CTL,   &acc_reg[3]));
    while (!syirDevice_READ_accReg(ACC_REG_FIFO_STATUS, &acc_reg[4]));
    while (!syirDevice_READ_accReg(ACC_REG_INT_SOURCE, &ACC_TOG));

    
    //---------------Laboratorium 3 -----------    
    syirDevice_ON_led(GREEN);
    syirDevice_OFF_led(ORANGE);
    
    //---------------Laboratorium 3 -----------
    uint8_t acc_ev = 0;
    uint8_t was_acc = 0;
    uint8_t read_num = 0;
    uint8_t acc_flag = 0;
    //---------------Laboratorium 3 -----------
    
    while (1){
        
        syirDevice_MEASURE_internalTemperature(&cpuM);
        cpuT  = syirDevice_EVAL_internalTemperature(cpuM);
        while (!syirDevice_READ_accReg(ACC_REG_INT_SOURCE, &ACC_TOG));
        
        
        // nasz kod 
        if(syirDevice_MEASURE_ambientTemperature(&external_measure_temp)){
           external_temp = syirDevice_EVAL_ambientTemperature(external_measure_temp); 
        }
        // Zadanie 4 
        cpuM_internal_temp[liczn] = cpuT; 
        external_temp_ar[liczn] = external_temp;
        voltage_measure[liczn] =  vddV;
        
        if(liczn == TEMP_SIZE)
        {
            liczn = 0;
        }else{
            liczn++;
        }

        //bubbleSort(cpuM_internal_temp,10);
        // ------- Lab 2 st ----------
        
        vddF1 = syirDevice_FULL_vdd(&vddF);

        // ------- Lab 2 end ----------
        
 
        // Filtry
        medianMCU_int = median_filter(cpuM_internal_temp,cpuT);        
        medianMCU_ext = median_filter(external_temp_ar,external_temp);
        median_voltage = median_filter(voltage_measure,vddV);
        log_len = 0;
        
        was_acc = syirDevice_EVENT_acc(&acc_ev); // czekam na event
        if (was_acc){
             while (!syirDevice_READ_accReg(ACC_REG_INT_SOURCE, &acc_reg[1]));
             acc_flag = 1;
        }
        if(vddF || acc_flag){
            
            //---------------Laboratorium 3 -----------
           
            while(!syirDevice_XYZ_acc(&acc_x, &acc_y, &acc_z));
            syirDevice_TOGGLE_led(GREEN); 
            
            float acc_x_f = (4.0*acc_x)/1024;
            float acc_y_f = (4.0*acc_y)/1024;
            float acc_z_f = (4.0*acc_z)/1024;
            
            acc_x_buf[acc_index] = acc_x_f;
            acc_y_buf[acc_index] = acc_y_f;
            acc_z_buf[acc_index] = acc_z_f;
            
            float median_acc_x_filter = median_filter(acc_x_buf, acc_x_f);
            float median_acc_y_filter = median_filter(acc_y_buf, acc_y_f);
            float median_acc_z_filter = median_filter(acc_z_buf, acc_z_f);
            
            acc_index = (acc_index + 1 ) % TEMP_SIZE;
            
            
            //---------------Laboratorium 3 -----------
            
            
            syirDevice_SET_deviceClock(SYIRDEVICE_CLOCK_INTERNAL_32MHz, SYIRDEVICE_CLOCK_DIV_1);
            syirDevice_MEASURE_vdd(&vddM);
            vddV = syirDevice_EVAL_vdd(vddM);
            log_len = 0;
            if (syirDevice_CHECK_N_systemClock(10, &log_ticks)) {
                bubbleSort(cpuM_internal_temp,10);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "-------------------------------------------------------- \r\n");
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "SYIR Device status: \r\n");
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "CPU internal temperature: %2.2f \r\n", cpuT);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "CPU external temperature: %2.2f \r\n", external_temp);
                   log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "CPU filtered median internal temp: %2.2f \r\n", medianMCU_int);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "ACC ID: %x BW_RATE: %x FIFO_CTL: %X POWER_CTL: %X FIFO_STATUS: %X \r\n", acc_reg[0], acc_reg[1], acc_reg[2], acc_reg[3], acc_reg[4]);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "ACC FIFO_STATUS: %x \r\n", syirDevice_STATUS_acc());
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "ACC x: %2.2f y: %2.2f z: %2.2f \r\n", (4.0*acc_x)/1024, (4.0*acc_y)/1024, (4.0*acc_z)/1024);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "ACC median data -> x: %2.2f y: %2.2f z: %2.2f \r\n",median_acc_x_filter , median_acc_y_filter, median_acc_z_filter);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "CPU filtered median external temp: %2.2f \r\n", medianMCU_ext);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "CPU filtered median voltage: %2.2f \r\n", median_voltage);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "SYIR Device status: \r\n");
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "VDD status: %2.2f   %s \r\n", vddV, vddF ? "BATTERY: FULL" : "BATTERY: CHARGING/WORKING", vddF);
                    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "was_acc status: %d \r\n", syirDevice_EVENT_acc(&acc_ev) );
                   log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "-------------------------------------------------------- \r\n");

            }        
            was_acc = 0;
            syirDevice_SEND_log(log_buf, log_len);
        
        
            if (syirDevice_CHECK_N_systemClock(10, &led_ticks)) {
                syirDevice_TOGGLE_led(GREEN);
                syirDevice_TOGGLE_led(ORANGE);
            }
        }else{
            acc_flag = 0;
            syirDevice_OFF_ports();
            syirDevice_ENTER_sleep();
            //syirDevice_SET_deviceClock(SYIRDEVICE_CLOCK_INTERNAL_32MHz, SYIRDEVICE_CLOCK_DIV_1);
            syirDevice_INIT_log();
            //syirDevice_INIT_acc();
            syirDevice_INIT_vdd(0, &vddF2);
        }
    }

    syirDevice_OFF_led(GREEN);
    syirDevice_OFF_led(ORANGE);
    while (1);
    return -1;
}
