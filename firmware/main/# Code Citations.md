# Code Citations

## License: Apache_2_0
https://github.com/espressif/ESP8266_RTOS_SDK/tree/015f30999c801d5c72209bdf6f06263213aac898/examples/wifi/roaming/main/roaming_example.c

```
event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
```


## License: unknown
https://github.com/ToraNova/esp32s2-usb2pppos/tree/682be2fe5893aeaafe137932acf4fe34549420ac/main/wifi_net.c

```
= "wifi_station";

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id ==
```


## License: unknown
https://github.com/grchibi/esp32-bme280/tree/48f17895f14322acc251b1154f67b791011dfdf2/main/main.c

```
if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "got ip:%s", ip4addr_ntoa(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
```


## License: unknown
https://github.com/VANTHO15/codeESP32/tree/8ecc971f6529d315a1aa889202cddb739b6a8dec/12end/main/app_config.c

```
, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT
```


## License: MIT
https://github.com/hkdickyko/hkdickyko.github.io/tree/f0c512dab036630e12ffa471c20023a52c10861e/_posts/2022-04-24-ESP32.md

```
);

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &
```

