#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"

#define TAG "HTTP"

#define MAX_HTTP_OUTPUT_BUFFER 2048

#define URL_BASE "https://api.thingspeak.com"
#define URL_WRITE "https://api.thingspeak.com/update"
#define KEY "5ZB65VGCLN1U6RR2"

static esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void httpInit() {
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    esp_http_client_config_t config = {
        .url = URL_BASE,
        .event_handler = _http_event_handle,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    ESP_ERROR_CHECK(esp_event_loop_init(_http_event_handle, NULL));
    esp_err_t err = esp_http_client_perform(client);

    // POST
    // const char *post_data = "field1=value1&field2=value2";
    // esp_http_client_set_url(client, "http://httpbin.org/post");
    // esp_http_client_set_method(client, HTTP_METHOD_POST);
    // esp_http_client_set_header(client, "Content-Type", "application/json");
    // esp_http_client_set_post_field(client, post_data, strlen(post_data));
    // err = esp_http_client_perform(client);
    // if (err == ESP_OK) {
    //     ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
    //             esp_http_client_get_status_code(client),
    //             esp_http_client_get_content_length(client));
    // } else {
    //     ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    // }

    // if (err == ESP_OK) {
    //     ESP_LOGI(TAG, "Status = %d, content_length = %d",
    //             esp_http_client_get_status_code(client),
    //             esp_http_client_get_content_length(client));
    // }
    esp_http_client_cleanup(client);
}

