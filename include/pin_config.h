#pragma once

// #define WIFI_SSID             "GL-MT1300-44e"
// #define WIFI_PASSWORD         "88888888"

#define WIFI_SSID             "xinyuandianzi"
#define WIFI_PASSWORD         "AA15994823428"

#define PIN_BOOT_BTN          0
#define PIN_GS1               1
#define PIN_GS2               2
#define PIN_GS3               3
#define PIN_GS4               10

#define EXAMPLE_LCD_H_RES     294
#define EXAMPLE_LCD_V_RES     126
#define LVGL_LCD_BUF_SIZE     (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)


#define WIFI_CONNECT_WAIT_MAX (30 * 1000)

#define NTP_SERVER1           "pool.ntp.org"
#define NTP_SERVER2           "time.nist.gov"
#define GMT_OFFSET_SEC        0
#define DAY_LIGHT_OFFSET_SEC  0
// if CUSTOM_TIMEZONE is not defined then TIMEZONE API used based on IP, check zones.h
// #define CUSTOM_TIMEZONE             "Europe/London"

/* Automatically update local time */
#define GET_TIMEZONE_API      "https://ipapi.co/timezone/"
