/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "connect.h"
#include "http.h"
#include "driver/gpio.h"
#include "ds18b20.h"

#define SENSOR_DOOR_PIN 25

const int TEMPERATURE_SENSOR_PIN = 14;

#define TAG_DOOR "DATA DOOR SENSOR"
#define TAG_TEMPERATURE "DATA TEMPERATURE SENSOR"

void configPresenceSensor()
{
  int isOn = 1;
  gpio_pad_select_gpio(SENSOR_DOOR_PIN);
  gpio_set_direction(SENSOR_DOOR_PIN, GPIO_MODE_INPUT);
  gpio_set_level(SENSOR_DOOR_PIN, isOn);
}

void sendDoorSensorReadingTask() {

  bool openDoor = false;
  uint32_t data = 0;

  while (1)
  {

    openDoor = false;

    do {
      data = gpio_get_level(SENSOR_DOOR_PIN);

      if (data == 1)
      {
        openDoor = true;
      }
      
    } while (ulTaskNotifyTake(pdTRUE, 500 / portTICK_PERIOD_MS) != pdTRUE);
      
    long ok = xQueueSend(queueDoorSensor, &openDoor, portMAX_DELAY);

    if(ok) {
      ESP_LOGI(TAG_DOOR, "SUCCESS");
    } else {
      ESP_LOGE(TAG_DOOR, "FAILURE");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  
}

void sendTemperatureSensorReadingTask() {
  uint32_t data = 0;

  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    data = ds18b20_get_temp();
    long ok = xQueueSend(queueTemperatureSensor, &data, portMAX_DELAY);

    if(ok) {
      ESP_LOGI(TAG_TEMPERATURE, "SUCCESS");
    } else {
      ESP_LOGE(TAG_TEMPERATURE, "FAILURE");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void printSensors()
{
  while (true)
  {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("Door sensor: %d\n", gpio_get_level(SENSOR_DOOR_PIN));
    printf("Temperature: %0.2f\n", ds18b20_get_temp());
  }
  
}

void app_main()
{
    ds18b20_init(TEMPERATURE_SENSOR_PIN);
    configPresenceSensor();
    ESP_ERROR_CHECK( nvs_flash_init() );
    wifiInit();

    queueTemperatureSensor = xQueueCreate(3, sizeof(uint32_t));
    queueDoorSensor = xQueueCreate(3, sizeof(bool));

    xTaskCreate(&http_get_task_thingspeak, "http_get_task_thingspeak", 4096, NULL, 5, NULL);
    xTaskCreate(&sendDoorSensorReadingTask, "sendDoorSensorReadingTask", 4096, NULL, 4, taskHandleDoorSensor);
    xTaskCreate(&sendTemperatureSensorReadingTask, "sendTemperatureSensorReadingTask", 4096, NULL, 3, taskHandleTemperatureSensor);
}
