/*
 * spi_HW.h
 *
 * Created: 2019-12-07 16:31:17
 *  Author: Måns Lindeberg
 */ 


#ifndef SPI_HW_H_
#define SPI_HW_H_


void configure_spi_master(void);

void configure_spi_master_callbacks(void);

void spi_write_read(uint8_t *tx_buffer,
uint16_t tx_len,
uint8_t *rx_buffer,
uint8_t rx_len);

uint8_t spi_read_byte();

void spi_write_array( uint16_t size, const uint8_t *const data);

void cs_high(void);
void cs_low(void);

#endif /* SPI_HW_H_ */