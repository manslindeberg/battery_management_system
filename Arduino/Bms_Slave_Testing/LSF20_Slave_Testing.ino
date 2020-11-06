n/************************************* Read me *******************************************
BMS - Arduino 
Lund Formula Student 2020
@Måns Lindeberg
*******************************************************************************************/

/************************* State Definitions ***************************


PASSIVE:     Waiting for respons from either charger or FCU wanting to turn on TS

CHARGING:    Charger is connected and BMS sends charge request every second so long the charger is ok.

START:       Starting the TS -> connecting AIRS and activate precharge/discharge circuit

MEASURE:     Measure battery variables when charging or active TS

HALT:        Locked state, needs manual reset somehow or watch-dog timer

SHUTDOWN:    Disconnects the battery pack from the inverter by open AIRS and discharge/precharge
                                                                        */

/************************* Includes ***************************/

#include <Arduino.h>
#include <stdint.h>
#include "LTC6810.h"
#include "bms_control.h"
#include "can_comm.h"
#include <TimerObject.h>
#include <SPI.h>


/************************* Defines *****************************/ 

/*Staets*/

uint8_t state;
#define START 1
#define MEASURE 2
#define SHUTDOWN 3
#define CHARGING 4
#define PASSIVE 5
#define HALT 6
#define CHARGING_TEST 8

/* Cell, Current and Temp Limits*/
#define CURRENT_LIMIT (150)
#define CELL_TEMP_L_LIMIT (0x3C)

/* Cell, Temperature time Limits*/
# define TC_MAX_PERIOD_VOLT (500)
# define TC_MAX_PEIOD_TEMP (1000)

/*Precharge and Discharge times*/
# define DISCHARGE_TIME (0x00)
# define PRECHARGE_TIME (0x00)

/*Ports Definitions*/
# define AIR_N_STATE (PC0)
# define AIR_P_STATE (PC1)
# define SDC_CURRENT (PC2)
# define HV_CURRENT  (PC3)
# define MCU_SDC_SIG (PC4)
# define MCU_TSAL    (PC5)
# define MCU_PREDIS  (PB0)
# define MCU_AIR_P   (PB7)
# define MCU_AIR_N   (PD7)
# define BMS_SDC     (PD4)
# define SS          (PB2)

/*Decleration of local functions*/
void chrg_timer_cb(void);
void(* resetFunc) (void) = 0; 
/*Global state variables for error etc.*/
bool ts_active;
bool chrg_state;
uint8_t temp_state;
uint8_t volt_state;
uint8_t amp_state;
uint8_t g_error;

/*Hardware Timer for CAN charger message every second*/
TimerObject *chrg_timer = new TimerObject(1000);

/*Hardware Timer for CAN charger message every second*/
TimerObject *send_data = new TimerObject(1000);

/*Timestamps for millis() timer*/
unsigned long t_amp;
unsigned long t_temp;
unsigned long t_volt;


/*Set-up*/
void setup() {
  
  Serial.begin(9600);
  Serial.print("State: SETUP");
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  if(!can_init()){
    systemReset();
  }
  g_error = slave_init();
  
  /*Initalizing timer. Temp and voltage single-shot callback.*/
  chrg_timer->setOnTimer(&chrg_timer_cb);
  send_data->setOnTimer(&send_volt_db);

  /*Arduino Board PIN MODE*/
  pinMode(MCU_AIR_P, OUTPUT);
  pinMode(MCU_AIR_N, OUTPUT);
  pinMode(MCU_PREDIS, OUTPUT);
  pinMode(BMS_SDC, OUTPUT); // 
  pinMode(SS,OUTPUT);
  //pinMode(SHDWN_CRCT, INPUT); // sdc_current_sensor
  pinMode(HV_CURRENT,OUTPUT);

  /*Initalizing state variables*/
  temp_state = 0;
  volt_state = 0;
  amp_state = 0;
  chrg_state = false;
  ts_active = false;
  
  /*Write and read configuration register*/
  //run_command(1);

  state = PASSIVE;

}


/*!*********************************************************************
  \brief main loop
  @return void
***********************************************************************/
void loop() {
  ts_active = false;
  
  /*Write and read configuration register*/
  //run_command(1);

  state = CHARGING_TEST;


  while(1) {
    switch (state)
    {

    
    case PASSIVE:
      Serial.print("State: PASSIVE");

      /*Some KIND of Initial start-up test should be checked here.*/
      while(1){
        
      /*if(get_error() == -1)
      {
        Serial.println("ERROR: SPI data corruption");
        systemReset();
        
      }*/
      
      if(chrg_state)
      {
        state = CHARGING;
      }

      if(get_ts_state())
      {
        state = START;
      }
     }

    case CHARGING_TEST:
      Serial.print("STATE: Charging");
      chrg_timer->Start();
      chrg_state = true;
      while(1){
        chrg_timer->Update();

        get_chrg_state();

          if (chrg_state)
          {
            /*Serial.println("state = true");
            if(millis() - get_chrg_timestamp()  > 3000)
            {
              Serial.println("WARNING: Charger Disconnected");
              chrg_state = false;
              state = PASSIVE;
              break;
            }
          
            if(can_rx_data.flag != 0)
            {
              chrg_timer->Stop();
              chrg_state = false;
              state = PASSIVE;
              break;
            }*/
          }else{
            Serial.println("state = false");
          }
        
        
      }
      break;

    break;
    
    case CHARGING:
      Serial.print("STATE: Charging");
      chrg_timer->Start();
      chrg_state = true;
      state = MEASURE;
      break;

      /*Activating TS- State*/
    case START:
      Serial.println("State: START");
      digitalWrite(MCU_AIR_N, LOW);
      digitalWrite(MCU_PREDIS, LOW);
      delay(250);
      digitalWrite(MCU_AIR_P, LOW);

      ts_active = true;
      state = MEASURE;
      break;

    
    
    /* Main Measuring state: temperature, current and voltage*/
    case MEASURE:
    
      send_data->Start(); 
      if (chrg_state)
      {
        Serial.print("State: MEASURE (CHARGING)");
      }
      else
      {
        Serial.print("State: MEASURE (TS ACTIVE)");
      }
              
      while (state == MEASURE)
      {

        send_data->Update();
        chrg_timer->Update();

        get_chrg_state();

          if (chrg_state)
          {
            if(millis() - get_chrg_timestamp()  > 3000)
            {
              Serial.println("WARNING: Charger Disconnected");
              chrg_state = false;
              state = PASSIVE;
              break;
            }
          
            if(can_rx_data.flag != 0)
            {
              chrg_timer->Stop();
              chrg_state = false;
              state = PASSIVE;
              break;
            } 
          }

        /*Start voltage ADC*/
        run_command(3);
        /*Start temperature ADC*/
        //run_command(5);
        /*
          * if(no_current_shutdown_loop){
          * state = IDLE;
          * break;
          * }
          */

        /*Reading Cell Data Registers*/
        run_command(4);

        /*Reading Cell Temperature Registers*/
        run_command(6);

        /*Getting checking CRC error from slaves*/
        g_error = get_error();
        
        /*Checking data corruption*/
        if (g_error != 0)
        {
          Serial.println("ERROR: SPI data corruption");
          state = PASSIVE;
          break;
        }
        
        /*If we're in charging mode we also need
        to check that everything is OK with charger.*/
        

        /*Check Cell Voltages*/
        if (!voltage_limit_check())
        {
          
          if (volt_state == 0)
          {
            /*Starts Timer 1*/
            t_volt = millis();
            volt_state = 1;
          }

          if (volt_state == 1)
          {
            if ((millis() - t_volt) > 500)
            {
              Serial.println("WARNING: Cell Voltages outside limit");
              Serial.print("      Shutdown time: ");
              Serial.print(millis() - t_volt, DEC);
              volt_state = 0;
              state= SHUTDOWN;
              break;
              
            }
          }
        }
        else
        {
          volt_state = 0;
        }

        /*Check Cell Temperature
        if (!temp_limit_check()) {
          if (temp_state == 0) {
            t_temp = millis();
            temp_state = 1;
          }
          if (temp_state == 1) {
            if ((millis() - t_temp) > 1000) {
              temp_state = 0;
              state = SHUTDOWN;
              break;
            }
          } else {
            temp_state = 0;
          }*/

          /*Check Current drawn from battery*/
          if(!current_limit_check())
          {
            Serial.println("WARNING: Too high current drawn from Battery");
            state = SHUTDOWN;
            break;
          }
      }
    break;


  case SHUTDOWN:
    Serial.print("State: SHUTDOWN");

    /* Stop delivering Torque to inverter CAN.*/
    digitalWrite(MCU_AIR_N, LOW);
    digitalWrite(MCU_AIR_P, LOW);
    digitalWrite(BMS_SDC, HIGH);
    digitalWrite(MCU_PREDIS, LOW);
    state = PASSIVE;
    break;



  case HALT:
    
    break;
  
    }
  }
}


/*Charger Callback function for CAN: Sending a heartbeat with information every second.*/
void chrg_timer_cb(){
    can_chrg_msg_tx();
}


void systemReset(){
Serial.println("System Reset in ");
delay(1000);
Serial.print(3);
Serial.print(", ");
delay(1000);
Serial.print(2);
Serial.print(", ");
delay(1000);
Serial.print(1);
resetFunc();  
}
