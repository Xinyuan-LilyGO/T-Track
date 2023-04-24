#include <Arduino.h>
#include "gc9107.h"
#include "lvgl.h"
#include "gui.h"
#include "SPI.h"

#include "pin_config.h"

#include "WiFi.h"
#include "OneButton.h"
#include "logo_img.h"

#include "sntp.h"
#include "time.h"

#include "zones.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

OneButton button1(PIN_BOOT_BTN, true);
const uint8_t dir_pins[4] = {PIN_GS1, PIN_GS2, PIN_GS3, PIN_GS4};

lv_obj_t *obj_point = NULL;

bool change_point = 0;
uint16_t indev_x, indev_y;

void wifi_test(void);
void timeavailable(struct timeval *t);
void printLocalTime();
void SmartConfig();
void setTimezone();

void lv_disp_flush(lv_disp_drv_t *disp,
                   const lv_area_t *area,
                   lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    // lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full); //Vertical display
    lcd_PushColors_SoftRotation(area->x1,
                                area->y1,
                                w,
                                h,
                                (uint16_t *)&color_p->full,
                                2); // Horizontal display
    lv_disp_flush_ready(disp);
}

static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (change_point != 0)
    {
        change_point = 0;
        data->point.x = indev_x;
        data->point.y = indev_y;
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setup()
{
    // put your setup code here, to run once:
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t *buf;

    pinMode(4, OUTPUT);
    digitalWrite(4, 1);

    pinMode(PIN_BOOT_BTN, INPUT_PULLUP);
    pinMode(PIN_GS1, INPUT_PULLUP);
    pinMode(PIN_GS2, INPUT_PULLUP);
    pinMode(PIN_GS3, INPUT_PULLUP);
    pinMode(PIN_GS4, INPUT_PULLUP);

    Serial.begin(115200);
    Serial.println("T-AMOLED-1_1 factory test procedure");
    Serial.printf("psram size : %d MB\r\nflash size : %d MB\r\n",
                  ESP.getPsramSize() / 1024 / 1024,
                  ESP.getFlashChipSize() / 1024 / 1024);

    gc9107_init();
    lcd_PushColors_SoftRotation(0, 0, 294, 126, (uint16_t *)gImage_logo_img, 2);
    delay(2000);

    lv_init();
    buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE, MALLOC_CAP_INTERNAL);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;

    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    lv_indev_drv_register(&indev_drv);

    wifi_test();
    sntp_servermode_dhcp(1); // (optional)
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
    setTimezone();
    gui_init();

    obj_point = lv_btn_create(lv_scr_act());
    lv_obj_set_size(obj_point, 10, 10);
    lv_obj_set_style_radius(obj_point, 90, 0);

    button1.attachClick(
        [](void *param)
        {
            // lv_obj_t *obj = (lv_obj_t *)param;
            // lv_obj_set_style_bg_color(
            //     obj,
            //     lv_color_make(random(0xff), random(0xff), random(0xff)),
            //     0);
            // change_point = true;
            lcd_sleep();
            digitalWrite(4, 0);
            uint64_t mask = 1 << PIN_BOOT_BTN;
            esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ALL_LOW);
            esp_deep_sleep_start();
        },
        obj_point);
}

void loop()
{
    // put your main code here, to run repeatedly:
    button1.tick();
    lv_timer_handler();
    delay(1);

    static bool last_dir[4];
    for (int i = 0; i < 4; i++)
    {
        bool dir = digitalRead(dir_pins[i]);
        if (dir != last_dir[i])
        {
            last_dir[i] = dir;

            switch (i)
            {
            case 0:
                if (indev_x < EXAMPLE_LCD_H_RES)
                    indev_x += 3;
                break;
            case 1:
                if (indev_y > 0)
                    indev_y -= 3;
                break;
            case 2:
                if (indev_x > 0)
                    indev_x -= 3;
                break;
            case 3:
                if (indev_y < EXAMPLE_LCD_V_RES)
                    indev_y += 3;
                break;
            default:
                break;
            }
            lv_obj_set_pos(obj_point, indev_x - 5, indev_y - 5);
        }
    }
}

void wifi_test(void)
{
    String text;

    lv_obj_t *log_label = lv_label_create(lv_scr_act());
    lv_obj_align(log_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_width(log_label, LV_PCT(100));
    lv_label_set_long_mode(log_label, LV_LABEL_LONG_SCROLL);
    lv_label_set_recolor(log_label, true);

    lv_label_set_text(log_label, "Scan WiFi...");
    LV_DELAY(1);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)
    {
        text = "no networks found";
    }
    else
    {
        text = n;
        text += " networks found\n";
        for (int i = 0; i < n; ++i)
        {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }
    lv_label_set_text(log_label, text.c_str());
    Serial.println(text);
    LV_DELAY(2000);
    text = "Connecting to ";
    Serial.print("Connecting to ");
    text += WIFI_SSID;
    text += "\n";
    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    uint32_t last_tick = millis();
    uint32_t i = 0;
    bool is_smartconfig_connect = false;
    lv_label_set_long_mode(log_label, LV_LABEL_LONG_WRAP);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        text += ".";
        lv_label_set_text(log_label, text.c_str());
        LV_DELAY(100);
        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        { /* Automatically start smartconfig when connection times out */
            text += "\nConnection timed out, start smartconfig";
            lv_label_set_text(log_label, text.c_str());
            LV_DELAY(100);
            is_smartconfig_connect = true;
            WiFi.mode(WIFI_AP_STA);
            Serial.println("\r\n wait for smartconfig....");
            text += "\r\n wait for smartconfig....";
            text += "\nPlease use #ff0000 EspTouch# Apps to connect to the "
                    "distribution network";
            lv_label_set_text(log_label, text.c_str());
            WiFi.beginSmartConfig();
            while (1)
            {
                LV_DELAY(100);
                if (WiFi.smartConfigDone())
                {
                    Serial.println("\r\nSmartConfig Success\r\n");
                    Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
                    Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
                    text += "\nSmartConfig Success";
                    text += "\nSSID:";
                    text += WiFi.SSID().c_str();
                    text += "\nPSW:";
                    text += WiFi.psk().c_str();
                    lv_label_set_text(log_label, text.c_str());
                    LV_DELAY(1000);
                    last_tick = millis();
                    break;
                }
            }
        }
    }
    if (!is_smartconfig_connect)
    {
        text += "\nCONNECTED \nTakes ";
        Serial.print("\n CONNECTED \nTakes ");
        text += millis() - last_tick;
        Serial.print(millis() - last_tick);
        text += " ms\n";
        Serial.println(" millseconds");
        lv_label_set_text(log_label, text.c_str());
    }
    LV_DELAY(2000);
    lv_obj_clean(lv_scr_act());
}

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("No time available (yet)");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
    Serial.println("Got time adjustment from NTP!");
    printLocalTime();
    WiFi.disconnect();
}

const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDzTCCArWgAwIBAgIQCjeHZF5ftIwiTv0b7RQMPDANBgkqhkiG9w0BAQsFADBa\n"
    "MQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJl\n"
    "clRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTIw\n"
    "MDEyNzEyNDgwOFoXDTI0MTIzMTIzNTk1OVowSjELMAkGA1UEBhMCVVMxGTAXBgNV\n"
    "BAoTEENsb3VkZmxhcmUsIEluYy4xIDAeBgNVBAMTF0Nsb3VkZmxhcmUgSW5jIEVD\n"
    "QyBDQS0zMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEua1NZpkUC0bsH4HRKlAe\n"
    "nQMVLzQSfS2WuIg4m4Vfj7+7Te9hRsTJc9QkT+DuHM5ss1FxL2ruTAUJd9NyYqSb\n"
    "16OCAWgwggFkMB0GA1UdDgQWBBSlzjfq67B1DpRniLRF+tkkEIeWHzAfBgNVHSME\n"
    "GDAWgBTlnVkwgkdYzKz6CFQ2hns6tQRN8DAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0l\n"
    "BBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwNAYI\n"
    "KwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j\n"
    "b20wOgYDVR0fBDMwMTAvoC2gK4YpaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL09t\n"
    "bmlyb290MjAyNS5jcmwwbQYDVR0gBGYwZDA3BglghkgBhv1sAQEwKjAoBggrBgEF\n"
    "BQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzALBglghkgBhv1sAQIw\n"
    "CAYGZ4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEB\n"
    "AAUkHd0bsCrrmNaF4zlNXmtXnYJX/OvoMaJXkGUFvhZEOFp3ArnPEELG4ZKk40Un\n"
    "+ABHLGioVplTVI+tnkDB0A+21w0LOEhsUCxJkAZbZB2LzEgwLt4I4ptJIsCSDBFe\n"
    "lpKU1fwg3FZs5ZKTv3ocwDfjhUkV+ivhdDkYD7fa86JXWGBPzI6UAPxGezQxPk1H\n"
    "goE6y/SJXQ7vTQ1unBuCJN0yJV0ReFEQPaA1IwQvZW+cwdFD19Ae8zFnWSfda9J1\n"
    "CZMRJCQUzym+5iPDuI9yP+kHyCREU3qzuWFloUwOxkgAyXVjBYdwRVKD05WdRerw\n"
    "6DEdfgkfCv4+3ao8XnTSrLE=\n"
    "-----END CERTIFICATE-----\n";

void setTimezone()
{
#ifdef CUSTOM_TIMEZONE
    String timezone = CUSTOM_TIMEZONE;
#else

    WiFiClientSecure *client = new WiFiClientSecure;
    String timezone;
    if (client)
    {
        client->setCACert(rootCACertificate);
        HTTPClient https;
        if (https.begin(*client, GET_TIMEZONE_API))
        {
            int httpCode = https.GET();
            if (httpCode > 0)
            {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK ||
                    httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    String payload = https.getString();
                    Serial.println(payload);
                    timezone = payload;
                }
            }
            else
            {
                Serial.printf("[HTTPS] GET... failed, error: %s\n",
                              https.errorToString(httpCode).c_str());
            }
            https.end();
        }
        delete client;
    }
#endif
    for (uint32_t i = 0; i < sizeof(zones); i++)
    {
        if (timezone == "none")
        {
            timezone = "CST-8";
            break;
        }
        if (timezone == zones[i].name)
        {
            timezone = zones[i].zones;
            break;
        }
    }

    Serial.println("timezone : " + timezone);
    setenv("TZ", timezone.c_str(), 1); // set time zone
    tzset();
}