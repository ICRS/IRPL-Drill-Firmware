#include <task.h>
#define BAUD_RATE 115200
TaskHandle_t samsCerealTaskHandle = nullptr;

// Takes a single message like "<MOTOR:20>" and outputs the correct message 
Message parseMessage(String input){
    Message output;
    String key;
    String value;
    if (input.startsWith("<") && input.endsWith(">")) {
        input = input.substring(1, input.length() - 1);
    } else {
        output.type = MessageType::ERROR;
        output.errorCode = 0; // Not a samsCereal message 
        return output;  
    }

    int colonIndex = input.indexOf(':');
    
    if (colonIndex!=-1){
        key = input.substring(0,colonIndex);
        value = input.substring(colonIndex+1);
        value.toLowerCase();
        key.toUpperCase();
    } else {
        output.type = MessageType::ERROR;
        output.errorCode = 1; // No key-value pair
        return output;  
    }

    // Big if else to sort message types (change to switch case with enums at some point)
    if (key=="PING"){
        output.type = MessageType::PING_IN;
        output.pingValue = (value=="true");
        String ping_out = "<PONG:"+ String(output.pingValue) +'>';
        Serial.println(ping_out);
    }
    else if (key=="MOTOR"){
        output.type = MessageType::MOTOR;
        output.motorValue = value.toFloat();
        drillMotor.setVelocity(output.motorValue);
        linearMotor.setVelocity(output.motorValue);
    }
    else if (key=="LINEAR"){
        output.type = MessageType::LINEAR;
        output.linearValue = value.toFloat();
        linearMotor.setVelocity(output.linearValue);
    }
    else if (key=="DRILL"){
        output.type = MessageType::DRILL;
        output.drillValue = value.toFloat();
        drillMotor.setVelocity(output.drillValue);
    }
    else if (key=="LC"){
        float masses[3];
        getLoadCellValues(masses);
        Serial.printf("<LC:%.2f,%.2f,%.2f>\n",masses[0],masses[1],masses[2]);
    }
    else if (key=="PAYLOAD"){
        output.type = MessageType::PAYLOAD;
        output.payloadValue = value.toInt();
        for (int i = 0; i < 15; ++i){
            inchPayload(output.payloadValue);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
    else if (key=="INCHPAY"){
        output.type = MessageType::INCHPAY;
        output.inchpayValue = value.toInt();
        inchPayload(output.inchpayValue);
    }
    else if (key=="BRUSH"){
        output.type = MessageType::BRUSH;
        output.brushValue = value.toInt();
        brushPos = output.brushValue;
    }
    else if (key=="LIMIT"){
        Serial.printf("%d\n", limitReached);
    }
    else {
        output.type = MessageType::ERROR;
        output.errorCode = 3; // Unknown key
    }

    return output;
}



String inputBuffer = "";

void samsCerealTask(void * parameter){
    Serial.begin(BAUD_RATE);
    Message incomingMessage;

    for(;;){
        String incoming;
        if (Serial.available() > 0){
            incoming = Serial.readStringUntil('\n');
            incomingMessage = parseMessage(incoming);

            if (incomingMessage.type == MessageType::ERROR){
                Serial.printf("Failed to parse message with code: %i\n", incomingMessage.errorCode);
                Serial.printf("Message that caused error = '%s'\n", incoming);
            }
        }

        /* Periodic position check. Stop moving down if at the maximum extension */
        float drill_position = linearMotor.getPosition();
        if (drill_position >= MAX_EXTENSION) linearMotor.setVelocity(0.0);
        Serial.printf("<POS:%.2f>\n", drill_position);
        vTaskDelay(pdMS_TO_TICKS(1000 / SAMS_CEREAL_FREQ));
    }
}