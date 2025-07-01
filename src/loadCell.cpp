#include <task.h>
#include <HX711.h>


HX711 lc2;



const long LC2_OFFSET = 0;
const long LC2_DIVIDER = 2135.4;





TaskHandle_t loadCellTaskHandle = nullptr;
volatile float loadCellValues[3];

void loadCellTask(void * parameter){
    Serial.println("Initialising LC2:");
    lc2.begin(LC2DT, LC2CLK);
    lc2.set_gain(128);
    lc2.read();
    lc2.set_scale(LC2_DIVIDER);
    lc2.set_offset(LC2_OFFSET);
    Serial.println("LC2 Finnished Initialising");
    for(;;){
        Serial.print("Weight: ");
        Serial.println((lc2.read()/LC2_DIVIDER)-1666);
        vTaskDelay((1000/LOADCELL_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}

