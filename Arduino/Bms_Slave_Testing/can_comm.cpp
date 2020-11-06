#include <mcp_can.h>
#include <SPI.h>
#include "can_comm.h"

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

/*Declaring local functions*/
void register_flag(void);
void MCP2515_ISR(void);
void print_chrg(void);


/*MSP2515 CS Pin on Arduino Shield*/
const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);   

const byte interruptPin = 0;
uint16_t time_stamp = 0;
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
bool chrg_active_flag = false;
bool ts_active_flag = false;


/* Struct for storing received data over CAN*/
volatile struct rx_data can_rx_data = {
    /*Charging*/
    .out_volt_high = 0,
    .out_volt_low = 0,
    .out_amp_high = 0,
    .out_amp_low = 0,
    .flag = 0xFF,
    .hw_fail = 2,
    .chrg_temp = 2,
    .in_volt = 2,
    .stating_state = 2,
    .comm_state = 2,
    
    /*FCU*/
    .start = 0

};

/*Struct for storing transmitting data over CAN*/
struct tx_data can_tx_data = {

    /*Charger*/
    .max_volt_high = 0,
    .max_volt_low = 42,
    .max_amp_high = 0,
    .max_amp_low = 150,
    .control = 0
};


/*Initalizing CAN module on Arduino board.*/
bool can_init(){
  
    while (CAN_OK != CAN.begin(CAN_250KBPS)) {            
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
        return false;
    }
    Serial.println("CAN BUS Shield init ok!");

    attachInterrupt(digitalPinToInterrupt(interruptPin), MCP2515_ISR, FALLING);
    
    /*set mask, set both the mask to 0x3ff*/
    CAN.init_Mask(0, 0, 0x3ff);                                   // there are 2 mask in mcp2515, you need to set both of them
    CAN.init_Mask(1, 0, 0x3ff);
    /*Maximum 6 filters*/
    CAN.init_Filt(0, 0, can_chrg_rx_id);
    CAN.init_Filt(1,0,can_fcu_rx_id);
    Serial.println("CAN BUS Shield init Sucessfull");
    return true;
}


/*Function for transmitting either standard length or extended length CAN frame*/
void can_transmit(uint32_t id, uint8_t tx_buffer[],uint8_t len, bool extended){
  
    if(extended){
      Serial.println("Transmitting Extended CAN-frame to ID: ");
      Serial.println(id, HEX);
      CAN.sendMsgBuf(id, 1, len,tx_buffer);
    }else{
      Serial.println("Transmitting Standard CAN-frame to ID: ");
      Serial.print(id, HEX);
      CAN.sendMsgBuf(id,0,len,tx_buffer);
    }
}


void MCP2515_ISR(){
    uint8_t ext = CAN.isExtendedFrame();
    uint32_t id = CAN.getCanId();
    Serial.print("CAN Message Received from ID:");
    Serial.print(id,HEX);
    CAN.readMsgBuf(&len,buf);
    
    switch(id){
        /*Message from Charger*/
        case can_chrg_rx_id:
        
          can_rx_data.out_volt_high = buf[0];
          can_rx_data.out_volt_low = buf[1];
          can_rx_data.out_amp_high = buf[2];
          can_rx_data.out_amp_low = buf[3];
          can_rx_data.flag = buf[4];

          time_stamp = millis();
          chrg_active_flag = true;
          register_flag();
          print_chrg();
          break;
          
        /*Message from FCU*/
        case can_fcu_rx_id:
          can_rx_data.start = buf[0];
          set_ts_state();
          break;

        default:
          for(int i=0; i < len; i++)
          {
            Serial.println(buf[i], DEC);
          }
          
        break;
   }
}

/*Setting flag registers from charger*/
void register_flag(){

    can_rx_data.hw_fail = (can_rx_data.flag & 0x01);
    can_rx_data.chrg_temp =  can_rx_data.flag & 0x02 >> 1;
    can_rx_data.in_volt =  (can_rx_data.flag & 0x04) >> 2;
    can_rx_data.stating_state =  (can_rx_data.flag & 0x08) >> 3;
    can_rx_data.comm_state =  (can_rx_data.flag & 0x10) >> 4;

}

void can_chrg_msg_tx(){
  uint8_t tx_buffer[5] = {can_tx_data.max_volt_high, can_tx_data.max_volt_low,
    can_tx_data.max_amp_high, can_tx_data.max_amp_low, can_tx_data.control };
    can_transmit(can_chrg_tx_id, tx_buffer,5, 1);
}

/*Function for getting the current mi*/
uint16_t get_chrg_timestamp(){
  return time_stamp;
}

/**/
bool get_chrg_state(){
    return chrg_active_flag;
}

/*Register TS start Request from FCU for state transition in state-machine*/
bool set_ts_state(){
    if(can_rx_data.start = 255)
    {
        ts_active_flag = true;
        Serial.println("TS ACTIVATE request received correctly from FCU");
    }    
}

/*Resets to false after each time function is called*/
bool get_ts_state(){
    bool reset = ts_active_flag;
    ts_active_flag = false;
    return reset;
}


/*Function for printing DATA from Charger*/
void print_chrg(){
    
    Serial.println("Output Voltage:  ");
    Serial.print((can_rx_data.out_volt_high << 8) + can_rx_data.out_volt_low, DEC);
    Serial.println("Output Current:  ");
    Serial.print((can_rx_data.out_amp_high << 8) + can_rx_data.out_amp_low, DEC);
    Serial.println("Hardware Failure: ");
    Serial.print(can_rx_data.hw_fail, DEC);
    Serial.println("Temperature of Charger:  ");
    Serial.print(can_rx_data.chrg_temp,DEC); 
    Serial.println("Input Voltage:  ");
    Serial.print(can_rx_data.in_volt,DEC);
    Serial.println("Stating State:  ");
    Serial.print(can_rx_data.stating_state,DEC);
    Serial.println("Communication State:  ");
    Serial.print(can_rx_data.comm_state, DEC);
            
}
