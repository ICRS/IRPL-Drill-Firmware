#include <task.h>

#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

Encoder drillEnc(ENC1A, ENC1A);
Encoder linearEnc(ENC2A, ENC2A);
TaskHandle_t encoderTaskHandle = nullptr;

#define BUFFER_SIZE 10

long positionDrill[BUFFER_SIZE];
int posIndexDrill = 0;
bool bufferFullDrill = false;
float speedDrill = 0.0;

long positionLinear[BUFFER_SIZE];
int posIndexLinear = 0;
bool bufferFullLinear = false;
float speedLinear = 0.0;

float distanceChange(int posIndex, long position[BUFFER_SIZE]) {
    int oldestIndex = posIndex;  // This is where the next new value will go => the oldest is here
    int newestIndex = (posIndex - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    return position[newestIndex] - position[oldestIndex];
}

void encoderTask(void *parameter){
    long prevAvgDrill = 0;
    long prevAvgLinear = 0;

    for (;;){
        // ------ DRILL ------
        long newPosDrill = drillEnc.read();
        positionDrill[posIndexDrill] = newPosDrill;
        posIndexDrill = (posIndexDrill + 1) % BUFFER_SIZE;
        if (posIndexDrill == 0) {
            bufferFullDrill = true;
        }
        if (bufferFullDrill) {
            long deltaDrill = distanceChange(posIndexDrill, positionDrill);

            speedDrill = deltaDrill / BUFFER_SIZE / 7.0f / 19.2f * 60 * ENCODER_TASK_FREQ;

            //Serial.print("Drill speed: ");
            //Serial.println(speedDrill);
        }
        Serial.print("Drill Position: ");
        Serial.println(newPosDrill);

        // ------ LINEAR ACTUATOR ------

        long newPosLinear = linearEnc.read();
        positionLinear[posIndexLinear] = newPosLinear;
        posIndexLinear = (posIndexLinear + 1) % BUFFER_SIZE;
        if (posIndexLinear == 0) {
            bufferFullLinear = true;
        }
        if (bufferFullLinear) {
            long deltaLinear = distanceChange(posIndexLinear, positionLinear);

            speedLinear = deltaLinear / BUFFER_SIZE / 7.0f / 369.0f * 60 * ENCODER_TASK_FREQ;

            //Serial.print("Linear speed: ");
            //Serial.println(speedLinear);
        }
        Serial.print("Linear Position: ");
        Serial.println(newPosLinear);

        
        
        vTaskDelay((1000 / ENCODER_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}
