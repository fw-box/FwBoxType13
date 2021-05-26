//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   None
//
// Connections :
//
// Required Library :
//   https://github.com/fw-box/FwBox_DualRfUart
//

#include "FwBox.h"
#include "FwBox_DualRfUart.h"


#define DEVICE_TYPE 13
#define FIRMWARE_VERSION "1.1.2"

#define VAL_INDEX_LED 0

//
// Global variable
//

FwBox_DualRfUart DualRfUart;

unsigned long ReadingTime = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  //
  // Initialize the fw-box core (early stage)
  //
  fbEarlyBegin(DEVICE_TYPE, FIRMWARE_VERSION);

  //
  // Initialize the fw-box core
  //
  fbBegin(DEVICE_TYPE, FIRMWARE_VERSION);

  FwBoxIns.setMqttMode(MQTT_MODE_PASSIVE);

  DualRfUart.begin();

} // void setup()

void loop()
{
  static unsigned long previous_receive_time = 0;

  if((millis() - ReadingTime) > 2000) {
#if DEBUG == 1
    Serial.print("Device UUID is ");
    Serial.println(FwBoxIns.getDeviceConfig()->Uuid);
    Serial.print("Device Type is ");
    Serial.println(FwBoxIns.getDeviceConfig()->Type);
#endif // #if DEBUG == 1

    ReadingTime = millis();
  }

  if(DualRfUart.handle() == true) {
    if((millis() - previous_receive_time) > 1000) {
      RF_DATA_FORMAT* rf_data = DualRfUart.getData();
      char buff[8];
      memset(&(buff[0]), 0, 8);
      DualRfUart.getAddressKey(&(buff[0]));
      //Serial.println(rf_data->FREQ);
      Serial.println(buff);
      if(rf_data->FREQ == DUAL_RF_UART_FREQ_315M) {
#if DEBUG == 1
        Serial.println("DUAL_RF_UART_FREQ_315M");
#endif // #if DEBUG == 1
        FwBoxIns.mqttPublish(0, &(buff[0]));
      }
      else if(rf_data->FREQ == DUAL_RF_UART_FREQ_433M) {
#if DEBUG == 1
        Serial.println("DUAL_RF_UART_FREQ_433M");
#endif // #if DEBUG == 1
        FwBoxIns.mqttPublish(1, &(buff[0]));
      }

      previous_receive_time = millis();
    }
  }

  //
  // Run the handle
  //
  fbHandle();

} // END OF "void loop()"
