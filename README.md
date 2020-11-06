# BMS - FSEA

## Introduction 
The evolution of the electric vehicle industry have excelled rapidly the last decade.  Intensive research on cell chemistries, accumulator container assemblies and electric motortechnologies  have  given  the  birth  of  Battery  Management  Systems(BMS)as  we  knowthem today.  Even though the field is quite young, researchers and industries are trying to find secure and reliable systems with the use of software and complex algorithms that can identify dangerous states in the battery container before an accident occurs.  This is primarily done with embedded software that, together with sensors-data, can ensure a safe operation of an electrical drive-train.  The main objective of the BMS is in other words to protect a battery container both short-term, that is; ensure safe current, voltage and temperature  values,  and  long-term  with  cell-balancing  mechanisms  that  ensures  longer life-time.  The long-term perspective is important since a battery pack is considered to be consumed in vehicle application when the state-of-health is between 70%-80%.  A BMS that ensures safe and sustainable pre-and discharges could therefore improve the life-time,which in turn helps to cut negative environmental effects that battery-production cause.

## Hardware
- Sensor Board
- Slave Board
- Master Board
- Current Sensor


The sensor borad is loosely speaking a PCB that covers a whole segment of cells in the accumulator, with the main objective to retrieve valuable data. That is, the voltage of each cell and the top-side temperature of every other cell in the battery pack. The benefit of having sensors that are placed directly on the PCB over having external sensors is that it provides better mechanical and electrical stability.

The AD-conversion of the signals from the sensor boards are then executed on slave-boards which carries the **LT6810** IC. The IC is a multi-cell battery stack monitor that is designed to evaluate the voltage and temperature of up to 6 battery cells and can be daisy-chained together to provide full cell cover of all serially connected cells in the pack. Furthermore, it has a build-in isoSPI interface that protects the signals from EMI and noise when reaching the Master board.

The Master is the brain of the BMS and uses a 32-bit **Atmel atSAMc21** based on the ARM Cortex M0+ that handles the SPI communication with the slaves, the CAN communication with the other MCU's on the card and provides the core function of the battery system as explained in the introduction, including shut-down algorithms and  charging monitoring. 

Lastly, a external current sensor (Hall-Effect Integrated Chip Sensor) is placed at the end-terminal on the battery-pack where the current is leaving. The signal is directly sent to the Master board which then performs the AD-conversion.

## System 
The building of the car is heavily regulated by the punctuations in the Rules Handbook, given out by Formula SEA. The Rules Handbook are a set of rules that the teams and cars must fulfil in order to be allowed to compete in the different competitions. The rules are divided into different subsystems, where Accumulator Management Systems are one of them (EV 5.8). The most important regulations regarding the design of the BMS software are:

   - EV 5.8.1 AMS must be active whenever the low-voltage system are on or when the battery pack is connected to a charger.
   - BMS must continously measure
        - all cell voltages
        - Current drawn by tractive system
        - temperature of thermally critical cells
        - temperature of at least 30\% of the cells equally distributed within the bettery container
   - The maximum cell temperature is 60 degree Celsius 
   - BMS must switch off tractive system via shutdown circuit if critical voltage, temperature or current values if these rules peristently occurs for more than 
        - 500 ms for voltage and current values
        - 1 s for temperature values
    - The accuracy, noise and sample rate of the measurement must be taken into account when applying the previous rule.
    - All BMS signals are system critical signals (SCS). This includes:
        - Following SCS failures must result in a safe state of all connected systems
            - Implausability due to out of range signals
            - Data corruption
            - Loss and delay of messages
    - The BMS must be able to read and display all measured values by connecting a laptop
