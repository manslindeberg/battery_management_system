

/*RX*/
#define can_chrg_rx_id (0x18FF50E5) // Extended
#define can_fcu_rx_id (0x15)        // Standard
/*TXT*/
#define can_chrg_tx_id (0x1806E5F4) // Extended

struct  rx_data
{
   /*Charger*/
    uint8_t out_volt_high;
    uint8_t out_volt_low;
    uint8_t out_amp_high;
    uint8_t out_amp_low;
    uint8_t flag;
    uint8_t hw_fail;
    uint8_t chrg_temp;
    uint8_t in_volt;
    uint8_t stating_state;
    uint8_t comm_state;

    uint8_t start;
};

/*Structure of the bytes we */
struct  tx_data
{
  /*Charger*/
  uint8_t max_volt_high;
  uint8_t max_volt_low;
  uint8_t max_amp_high;
  uint8_t max_amp_low;
  uint8_t control;
  
};

extern volatile struct rx_data can_rx_data;
extern struct tx_data can_tx_data;

void can_chrg_msg_tx(void);
bool can_init(void);
uint16_t get_chrg_timestamp(void);
bool get_chrg_state(void);
bool set_ts_state(void);
bool get_ts_state(void);
