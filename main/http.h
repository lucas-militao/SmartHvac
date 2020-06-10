#ifndef _HTTP_H_
#define _HTTP_H_

void http_get_task_thingspeak(void *pvParameters);
extern xQueueHandle queueDoorSensor;
extern xQueueHandle queueTemperatureSensor;

extern TaskHandle_t taskHandleDoorSensor;
extern TaskHandle_t taskHandleTemperatureSensor;

#endif