/*
 * spi_HW.c
 *
 * Created: 2019-12-07 16:16:25
 *  Author: Måns Lindeberg
 */ 

#include <asf.h>
#include "spi_HW.h"

#define SLAVE_SELECT_PIN CONF_MASTER_SS_PIN

struct spi_module spi_master_instance;

struct spi_slave_inst slave;

volatile bool transrev_complete_spi_master = false;


/* Internal Help Function for transmit.*/
static void callback_spi_master( struct spi_module *const module)
{
	transrev_complete_spi_master = true;
}

void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;

	/* Configure and initialize software device instance of peripheral slave. */
	
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	spi_attach_slave(&slave, &slave_dev_config);
	spi_get_config_defaults(&config_spi_master);
	
	config_spi_master.mux_setting = CONF_MASTER_MUX_SETTING;
	config_spi_master.pinmux_pad0 = CONF_MASTER_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = CONF_MASTER_PINMUX_PAD1;
	config_spi_master.pinmux_pad2 = CONF_MASTER_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = CONF_MASTER_PINMUX_PAD3;

	spi_init(&spi_master_instance, CONF_MASTER_SPI_MODULE, &config_spi_master);
	spi_enable(&spi_master_instance);
	
	configure_spi_master_callbacks();

}
/* Internal Help function to  configure SPI master callback function.*/
void configure_spi_master_callbacks(void)
{
	spi_register_callback(&spi_master_instance, callback_spi_master,
	SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
}



void spi_write_read(uint8_t tx_buffer[], 
					uint16_t tx_len, 
					uint8_t rx_buffer[],
					uint8_t rx_len){

	spi_select_slave(&spi_master_instance, &slave, true);
	spi_transceive_buffer_job(&spi_master_instance, tx_buffer,rx_buffer,tx_len);

	while (!transrev_complete_spi_master)
	{
		/*Waiting for transfer*/
	}
		transrev_complete_spi_master = false;
		spi_select_slave(&spi_master_instance, &slave, false);
}

void spi_write_array(uint16_t size,const uint8_t *const data)
{
		spi_select_slave(&spi_master_instance, &slave, true);
		spi_write_buffer_wait(&spi_master_instance, data, size);
		spi_select_slave(&spi_master_instance, &slave, false);
	
}

uint8_t spi_read_byte(){
	
	uint8_t rx_byte[1] = {0x00};
	uint16_t length = 0x01;
	spi_select_slave(&spi_master_instance, &slave, true);
	spi_read_buffer_wait(&spi_master_instance,rx_byte,length, 0x00);
	spi_select_slave(&spi_master_instance, &slave, false);
	
	return rx_byte[0];
	
	
}

void cs_high(void)
{
	spi_select_slave(&spi_master_instance, &slave, false);
}
void cs_low(void)
{
	spi_select_slave(&spi_master_instance, &slave, true);
}