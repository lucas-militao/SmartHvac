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

void configPresenceSensor()
{
  int isOn = 1;
  gpio_pad_select_gpio(SENSOR_DOOR_PIN);
  gpio_set_direction(SENSOR_DOOR_PIN, GPIO_MODE_INPUT);
  gpio_set_level(SENSOR_DOOR_PIN, isOn);
}

void app_main()
{
    ds18b20_init(TEMPERATURE_SENSOR_PIN);
    configPresenceSensor();
    ESP_ERROR_CHECK( nvs_flash_init() );
    wifiInit();
    xTaskCreate(&http_get_task_thingspeak, "http_get_task_thingspeak", 4096, NULL, 5, NULL);
}
