#ifndef _CONNECT_H_
#define _CONNECT_H_

void wifiInit();
extern const int CONNECTED_BIT;
extern EventGroupHandle_t wifi_event_group;

#endif