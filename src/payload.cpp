#include <task.h>
#include <Deneyap_Servo.h> 
#include <HX711_ADC.h>

TaskHandle_t payloadTaskHandle = nullptr;

Servo servo1, servo2;

#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 10; //mcu > HX711 dout pin
const int HX711_sck = 11; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_calVal_eepromAdress = 0;

const int calVal_eepromAdress = 0;
unsigned long t = 0;

bool toggle = false;

// Forward declarations
void calibrate();
void changeSavedCalFactor();

void payloadTask(void *parameter) {
    servo1.attach(SERVO1);

    Serial.begin(57600); delay(10);
    Serial.println();
    Serial.println("Starting...");

    float calibrationValue; // calibration value
    calibrationValue = -2213.69; // uncomment this if you want to set this value in the sketch

    LoadCell.begin();
    //LoadCell.setReverseOutput();
    unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
    LoadCell.start(stabilizingtime, _tare);
    if (LoadCell.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    }
    else {
        LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
        Serial.println("Startup is complete");
    }
    while (!LoadCell.update());
    Serial.print("Calibration value: ");
    Serial.println(LoadCell.getCalFactor());
    Serial.print("HX711 measured conversion time ms: ");
    Serial.println(LoadCell.getConversionTime());
    Serial.print("HX711 measured sampling rate HZ: ");
    Serial.println(LoadCell.getSPS());
    Serial.print("HX711 measured settlingtime ms: ");
    Serial.println(LoadCell.getSettlingTime());
    Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
    if (LoadCell.getSPS() < 7) {
        Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
    }
    else if (LoadCell.getSPS() > 100) {
        Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
    }

    for (;;) {
        if (toggle) {
            servo1.write(150); // set servo to 90 degrees
            toggle = false;
        } else {
            servo1.write(0); // set servo to 0 degrees
            toggle = true;
        }

        static boolean newDataReady = 0;
        const int serialPrintInterval = 500; //increase value to slow down serial print activity

        // check for new data/start next conversion:
        if (LoadCell.update()) newDataReady = true;

        // get smoothed value from the dataset:
        if (newDataReady) {
            if (millis() > t + serialPrintInterval) {
            float i = LoadCell.getData();
            Serial.print("Load_cell output val: ");
            Serial.println(i);
            newDataReady = 0;
            t = millis();
            }
        }

        // receive command from serial terminal, send 't' to initiate tare operation:
        if (Serial.available() > 0) {
            char inByte = Serial.read();
            if (inByte == 't') LoadCell.tareNoDelay();
        }

        // check if last tare operation is complete:
        if (LoadCell.getTareStatus() == true) {
            Serial.println("Tare complete");
        }
        
        vTaskDelay((1000 / PAYLOAD_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}

// void payloadTask(void *parameter) {
//     Serial.begin(57600); delay(10);
//     Serial.println();
//     Serial.println("Starting...");

//     LoadCell.begin();
//     //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
//     unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
//     boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
//     LoadCell.start(stabilizingtime, _tare);
//     if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
//         Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
//         while (1);
//     }
//     else {
//         LoadCell.setCalFactor(1.0); // user set calibration value (float), initial value 1.0 may be used for this sketch
//         Serial.println("Startup is complete");
//     }
//     while (!LoadCell.update());
//         calibrate(); //start calibration procedure
//     for (;;) {
//         static boolean newDataReady = 0;
//         const int serialPrintInterval = 0; //increase value to slow down serial print activity

//         // check for new data/start next conversion:
//         if (LoadCell.update()) newDataReady = true;

//         // get smoothed value from the dataset:
//         if (newDataReady) {
//             if (millis() > t + serialPrintInterval) {
//             float i = LoadCell.getData();
//             Serial.print("Load_cell output val: ");
//             Serial.println(i);
//             newDataReady = 0;
//             t = millis();
//             }
//         }

//         // receive command from serial terminal
//         if (Serial.available() > 0) {
//             char inByte = Serial.read();
//             if (inByte == 't') LoadCell.tareNoDelay(); //tare
//             else if (inByte == 'r') calibrate(); //calibrate
//             else if (inByte == 'c') changeSavedCalFactor(); //edit calibration value manually
//         }

//         // check if last tare operation is complete
//         if (LoadCell.getTareStatus() == true) {
//             Serial.println("Tare complete");
//         }
//         vTaskDelay((1000 / PAYLOAD_TASK_FREQ) / portTICK_PERIOD_MS);
//     }
// }

void calibrate() {
  Serial.println("***");
  Serial.println("Start calibration:");
  Serial.println("Place the load cell an a level stable surface.");
  Serial.println("Remove any load applied to the load cell.");
  Serial.println("Send 't' from serial monitor to set the tare offset.");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') LoadCell.tareNoDelay();
      }
    }
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
      _resume = true;
    }
  }

  Serial.println("Now, place your known mass on the loadcell.");
  Serial.println("Then send the weight of this mass (i.e. 100.0) from serial monitor.");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }

  LoadCell.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass); //get the new calibration value

  Serial.print("New calibration value has been set to: ");
  Serial.print(newCalibrationValue);
  Serial.println(", use this as calibration value (calFactor) in your project sketch.");
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");

  _resume = false;
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);
#endif
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.commit();
#endif
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Value ");
        Serial.print(newCalibrationValue);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(calVal_eepromAdress);
        _resume = true;

      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        _resume = true;
      }
    }
  }

  Serial.println("End calibration");
  Serial.println("***");
  Serial.println("To re-calibrate, send 'r' from serial monitor.");
  Serial.println("For manual edit of the calibration value, send 'c' from serial monitor.");
  Serial.println("***");
}

void changeSavedCalFactor() {
  float oldCalibrationValue = LoadCell.getCalFactor();
  boolean _resume = false;
  Serial.println("***");
  Serial.print("Current value is: ");
  Serial.println(oldCalibrationValue);
  Serial.println("Now, send the new value from serial monitor, i.e. 696.0");
  float newCalibrationValue;
  while (_resume == false) {
    if (Serial.available() > 0) {
      newCalibrationValue = Serial.parseFloat();
      if (newCalibrationValue != 0) {
        Serial.print("New calibration value is: ");
        Serial.println(newCalibrationValue);
        LoadCell.setCalFactor(newCalibrationValue);
        _resume = true;
      }
    }
  }
  _resume = false;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);
#endif
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.commit();
#endif
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Value ");
        Serial.print(newCalibrationValue);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(calVal_eepromAdress);
        _resume = true;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        _resume = true;
      }
    }
  }
  Serial.println("End change calibration value");
  Serial.println("***");
}