#include <HX711.h>
#include <task.h>

HX711 lc1;  // HX711 for LC1 (channel A + B)
HX711 lc2;  // HX711 for LC2 (single load cell)

TaskHandle_t loadCellTaskHandle = nullptr;
volatile float loadCellValues[3];   // [0]=LC1 A, [1]=LC1 B, [2]=LC2

float LC1A_SCALE = -2075.353396f;
long  LC1A_OFFSET = 5738782;

// works
float LC1B_SCALE = -529.4645419f;
long  LC1B_OFFSET = 718894;


// TODO
float LC2_SCALE = 463.1009717f;
long  LC2_OFFSET = -469205;

void getLoadCellValues(float *target_array) {
    if (xSemaphoreTake(loadCellMutex, portMAX_DELAY)) {
        for (int i = 0; i < 3; i++) {
            target_array[i] = loadCellValues[i];
        }
        xSemaphoreGive(loadCellMutex);
    }
}

void calibrateLoadCell(int lc_num) {
    HX711 *lc = nullptr;
    int gain = 128;

    switch (lc_num) {
        case 1:
            lc = &lc1;
            gain = 128; // channel A
            break;
        case 2:
            lc = &lc1;
            gain = 32; // channel B
            break;
        case 3:
            lc = &lc2;
            gain = 128;
            break;
        default:
            Serial.println("Invalid LC number");
            return;
    }

    Serial.print("Calibrating LC");
    Serial.println(lc_num);

    lc->set_gain(gain);
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.println("Remove all weight...");
    vTaskDelay(pdMS_TO_TICKS(3000));
    lc->tare();

    Serial.println("Place known weight...");
    vTaskDelay(pdMS_TO_TICKS(5000));

    float knownWeight = 117.3;
    long reading = lc->get_value(20);
    float scale = (float)(reading) / knownWeight;

    switch (lc_num) {
        case 1:
            LC1A_OFFSET = lc->get_offset();
            LC1A_SCALE = scale;
            break;
        case 2:
            LC1B_OFFSET = lc->get_offset();
            LC1B_SCALE = scale;
            break;
        case 3:
            LC2_OFFSET = lc->get_offset();
            LC2_SCALE = scale;
            break;
    }

    Serial.println("Calibration complete for LC" + String(lc_num));
    Serial.println("Scale: " + String(scale, 6));
    Serial.println("Offset: " + String(lc->get_offset()));

}

void loadCellTask(void *parameter) {
    Serial.println("Initialising HX711s...");

    lc1.begin(LC1DT, LC1CLK);
    lc2.begin(LC2DT, LC2CLK);

    Serial.println("Load cells initialised");

    const TickType_t xPeriod = pdMS_TO_TICKS(1000 / LOADCELL_TASK_FREQ);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        if (xSemaphoreTake(loadCellMutex, portMAX_DELAY)) {
            lc1.set_gain(128);
            loadCellValues[0] = (lc1.get_value() - LC1A_OFFSET) / LC1A_SCALE;

            lc1.set_gain(32);
            loadCellValues[1] = (lc1.get_value() - LC1B_OFFSET) / LC1B_SCALE;

            lc2.set_gain(128);
            loadCellValues[2] = (lc2.get_value() - LC2_OFFSET) / LC2_SCALE;

            xSemaphoreGive(loadCellMutex);
        }
        xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}