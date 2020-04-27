#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "connect.h"
#include "driver/gpio.h"
#include "ds18b20.h"
#include "esp_http_client.h"
#include "http.h"

#define SENSOR_DOOR_PIN 25 

const int TEMPERATURE_SENSOR_PIN = 14;

xQueueHandle readingQueue;
TaskHandle_t taskHandle;
int abacaxi = 0;

const uint32_t WIFI_CONNEECTED = BIT1;

void Logic() {
  uint32_t command = 0;

  while(true) {
    xTaskNotifyWait(0,0,&command, portMAX_DELAY);
    switch (command) {
      case WIFI_CONNEECTED:
        ESP_LOGI("WIFI", "CONECTADO");
        break;
      default:
        break;
    }
  }
}

void OnConnected(void *para) {
  while(true)
  {
    int temperatura;
    if(xQueueReceive(readingQueue, &temperatura, portMAX_DELAY)) {
      ESP_ERROR_CHECK(esp_wifi_start());
      Logic();
    }
  }
}

void generateReading(void *params) {
  while(true) {
    int temperatura = ds18b20_get_temp();
    xQueueSend(readingQueue, &temperatura, 2000 / portTICK_PERIOD_MS);
  }
}

void configPresenceSensor()
{
  int isOn = 1;
  gpio_pad_select_gpio(SENSOR_DOOR_PIN);
  gpio_set_direction(SENSOR_DOOR_PIN, GPIO_MODE_INPUT);
  gpio_set_level(SENSOR_DOOR_PIN, isOn);
}

void app_main(void)
{
  ds18b20_init(TEMPERATURE_SENSOR_PIN);
  configPresenceSensor();
  readingQueue = xQueueCreate(sizeof(int), 10);
  wifiInit();
  httpInit();
  xTaskCreate(OnConnected, "conexaoWIfi", 1024 * 5, NULL, 5, &taskHandle);
  xTaskCreate(generateReading, "Leitura", 1024 * 5, NULL, 5, NULL);
}
