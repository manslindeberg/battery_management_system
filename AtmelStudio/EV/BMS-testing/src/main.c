/*
 * main.c
 *
 * Created: 2019-11-26 16:16:25
 *  Author: Måns Lindeberg
 */ 
#include <asf.h>
#include <stdint.h>
#include "HW\spi_HW.h"
#include "HW\uart_HW.h"
#include "HW\tc_HW.h"
#include "LT\LTC6810.h"

#define BUF_LENGTH (10)
void print_cells(void);
void setup(void);
void print_rxconfig();


static uint8_t wr_buffer[BUF_LENGTH] = {
 		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
bool temp;
volatile uint8_t rd_buffer[BUF_LENGTH];

/************************* Defines *****************************/
#define ENABLED 1
#define DISABLED 0
#define DATALOG_ENABLED 1
#define DATALOG_DISABLED 0


/********************************************************************
  Setup Variables
  The following variables can be modified to configure the software.
*********************************************************************/
const uint8_t TOTAL_IC = 2;//!< Number of ICs in the daisy chain

//ADC Command Configurations. See LTC681x.h for options.
const uint8_t ADC_OPT = ADC_OPT_DISABLED; //!< ADC Mode option bit
const uint8_t ADC_CONVERSION_MODE = MD_7KHZ_3KHZ; //!< ADC Mode
const uint8_t ADC_DCP = DCP_DISABLED; //!< Discharge Permitted 
const uint8_t CELL_CH_TO_CONVERT = CELL_CH_ALL; //!< Channel Selection for ADC conversion
const uint8_t AUX_CH_TO_CONVERT = AUX_CH_ALL; //!< Channel Selection for ADC conversion
const uint8_t STAT_CH_TO_CONVERT = STAT_CH_ALL; //!< Channel Selection for ADC conversion
const uint8_t SEL_ALL_REG = REG_ALL; //!< Register Selection 
const uint8_t SEL_REG_A = REG_1; //!< Register Selection 
const uint8_t SEL_REG_B = REG_2; //!< Register Selection 

//Under Voltage and Over Voltage Thresholds
const uint16_t OV_THRESHOLD = 41000; //!< Over voltage threshold ADC Code. LSB = 0.0001 ---(4.1V)
const uint16_t UV_THRESHOLD = 30000; //!< Under voltage threshold ADC Code. LSB = 0.0001 ---(1V)

const uint16_t MEASUREMENT_LOOP_TIME = 500; //!< Loop Time in milliseconds(ms)

//Loop Measurement Setup. These Variables are ENABLED or DISABLED Remember ALL CAPS
const uint8_t WRITE_CONFIG = DISABLED; //!< This is to ENABLED or DISABLED writing into to configuration registers in a continuous loop
const uint8_t READ_CONFIG = DISABLED; //!< This is to ENABLED or DISABLED reading the configuration registers in a continuous loop
const uint8_t MEASURE_CELL = ENABLED; //!< This is to ENABLED or DISABLED measuring the cell voltages in a continuous loop
const uint8_t MEASURE_AUX = DISABLED; //!< This is to ENABLED or DISABLED reading the auxiliary registers in a continuous loop
const uint8_t MEASURE_STAT = DISABLED; //!< This is to ENABLED or DISABLED reading the status registers in a continuous loop
const uint8_t PRINT_PEC = DISABLED; //!< This is to ENABLED or DISABLED printing the PEC Error Count in a continuous loop
/************************************
  END SETUP
*************************************/

/******************************************************
 Global Battery Variables received from 681x commands
 These variables store the results from the LTC6810
 register reads and the array lengths must be based
 on the number of ICs on the stack
 ******************************************************/
cell_asic BMS_IC[1];//!< Global Battery Variable

bool ISOSPI_DIR = false;

/*************************************************************************
 Set configuration register. Refer to the data sheet
**************************************************************************/
bool ADCOPT = false; //!< ADC Mode option bit
bool REFON = true; //!< Reference Powered Up Bit
bool GPIOBITS[4] = {false,true,true,true}; //!< GPIO Pin Control // Gpio 1,2,3,4
uint16_t UV = 41000; //!< Under voltage Comparison Voltage
uint16_t OV = 30000; //!< Over voltage Comparison Voltage
bool DCCBITS[6] = {false,false,false,false,false,false}; //!< Discharge cell switch //Dcc 1,2,3,4,5,6
bool DCCBIT_0 = false; //!< Discharge cell switch //Dcc 0 // For discharging optional 7th cell
bool MCAL = false; //!< Enable Multi-Calibration
bool EN_DTMEN = true; //!< Enable Discharge timer monitor
bool DIS_RED = false; //!< Disable Digital Redundancy Check
bool FDRF = false; //!< Force digital Redundancy Failure
bool SCONV = true; //!< Enable Cell Measurement Redundancy using S Pin
bool DCTOBITS[4] = {true,false,true,false}; //!<  Discharge Time Out Value //Dcto 0,1,2,3 // Programed for 4 min 
/*Ensure that Dcto bits are set according to the required discharge time. Refer to the data sheet */
int8_t err = 0;
/*!**********************************************************************
 \brief  Initializes hardware and variables
 @return void
 ***********************************************************************/
void setup()
{
  LTC6810_init_cfg(TOTAL_IC, BMS_IC);
  for (uint8_t current_ic = 0; current_ic<TOTAL_IC; current_ic++) 
  {
    LTC6810_set_cfgr(current_ic, BMS_IC, REFON, ADCOPT, GPIOBITS, DCCBITS, DCCBIT_0, MCAL, EN_DTMEN, DIS_RED, FDRF, SCONV, DCTOBITS, UV, OV);   
  }   
  LTC6810_reset_crc_count(TOTAL_IC,BMS_IC);
  LTC6810_init_reg_limits(TOTAL_IC,BMS_IC);
}

 int main(void)
 {
// 	/* Initialize system */
 
 	system_init();
 	board_init();
 	configure_spi_master();
 	configure_usart();
	configure_usart_callbacks();
 	system_interrupt_enable_global();
 	delay_init();
	//setup();
	
	//wakeup_sleep(TOTAL_IC);
	//LTC6810_wrcfg(TOTAL_IC,BMS_IC); // Write into Configuration Register
	
	uint8_t test_buffer[] = {0x23,0x34,0x34,0x34,0x54};
 	temp = false;
 	while (true) {
 		if (!port_pin_get_input_level(BUTTON_0_PIN)) {
 			port_pin_set_output_level(LED_0_PIN, LED0_ACTIVE);
			 uart_transmit(test_buffer,sizeof(test_buffer));
			// uart_transmit((uint8_t*)"Hello World", strlen("Hello World"), true);
			//uart_transmit("This is a test", sizeof("This is a test"), true);
			//uart_transmit(test_buffer, sizeof(test_buffer), false);
			 // Start Cell ADC measurements
		    //wakeup_sleep(TOTAL_IC);
			//LTC6810_adcv(ADC_CONVERSION_MODE,ADC_DCP,CELL_CH_TO_CONVERT);
			
			//Read Cell voltages
			//wakeup_sleep(TOTAL_IC);
			//err = LTC6810_rdcv(SEL_ALL_REG,TOTAL_IC,BMS_IC);
			
			//print cells:
			//print_rxconfig();
 			delay_cycles_ms(500);
 		}else{
 			port_pin_set_output_level(LED_0_PIN, !LED0_ACTIVE);	
 		}
 	}
 }
 
// void print_cells(){
// 	for(uint8_t i = 0; i < BMS_IC[0].ic_reg.cell_channels; i++)
// 	{
// 		uint8_t cell = BMS_IC[0].cells.c_codes[i]*0.0001;
// 		uart_transmit(cell, 1,false);
// 	}
// }

// void print_rxconfig(){
// 	uart_transmit("Received Configuration", strlen("Received Configuration"),true);
// 	for (int current_ic=0; current_ic<TOTAL_IC; current_ic++)
// 	{
// 		uart_transmit("CFGA IC ", sizeof("CFGA IC "),true);
// 		char buffer[200];
// 		uart_transmit(itoa(current_ic+1,buffer,10),sizeof(buffer),false);
// 		for(int i = 0; i < 6; i++)
// 		{			
// 			uart_transmit(BMS_IC[current_ic].config.rx_data[i],1,false);
// 		}
// 		uart_transmit(", Received PEC:", sizeof(", Received PEC: 0x"),true);
// 		
// 		uart_transmit(BMS_IC[current_ic].config.rx_data[6],1,false);
// 		
// 		uart_transmit(BMS_IC[current_ic].config.rx_data[7],1,false);
// 		uart_transmit("\r\n", sizeof("\r\n"),true);
// 	}
// }
