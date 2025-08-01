#include <task.h>
#include <HX711.h>
HX711 lc2;

const long LC2_OFFSET = -1500;
const long LC2_DIVIDER = 2135.4;

TaskHandle_t loadCellTaskHandle = nullptr;
volatile float loadCellValues[3];

void getLoadCellValues(float *target_array)
{
    if (xSemaphoreTake(loadCellMutex, portMAX_DELAY))
    {
        for (int i = 0; i < 3; i++)
        {
            target_array[i] = loadCellValues[i]; // Copy safely
        }
        xSemaphoreGive(loadCellMutex);
    }
}

void loadCellTask(void *parameter)
{
    Serial.println("Initialising LC2:");
    lc2.begin(LC2DT, LC2CLK);
    lc2.set_gain(128);
    lc2.set_offset(LC2_OFFSET);
    lc2.set_scale(LC2_DIVIDER);
    Serial.println("LC2 Finnished Initialising");

    const TickType_t xPeriod = pdMS_TO_TICKS(1000 / LOADCELL_TASK_FREQ);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {   
        if (xSemaphoreTake(loadCellMutex, portMAX_DELAY))
        {
            loadCellValues[1] = (lc2.get_units());
            xSemaphoreGive(loadCellMutex);
        }
        xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}
