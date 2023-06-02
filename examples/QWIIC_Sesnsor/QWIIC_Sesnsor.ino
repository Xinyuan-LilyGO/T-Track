/**
 * @file      QWIIC_Sesnsor.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-05-27
 * @note      As a Qwiic interface demonstration, the sketch only demonstrates the MPU9250 and BME280 sensors
 */

#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include "JD9613.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#ifndef BOARD_HAS_PSRAM
#error "Detected that PSRAM is not turned on. Please set PSRAM to OPI PSRAM in ArduinoIDE"
#endif //BOARD_HAS_PSRAM

#define POWER_ON_PIN          4
#define PIN_BOOT_BTN          0
#define PIN_GS1               1
#define PIN_GS2               2
#define PIN_GS3               3
#define PIN_GS4               10
#define TRACK_QWIIC_SDA       43
#define TRACK_QWIIC_SCL       44

#define SEALEVELPRESSURE_HPA (1013.25)

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

#include <MPU9250.h>
MPU9250 IMU(Wire, 0x68);
uint32_t measurementInterval = 0;
lv_obj_t *sensor_label;

bool sensorOnline_BME280 = false;
bool sensorOnline_MPU9250 = false;
uint8_t sensor_addr_BMX280 = 0x77;
static void lvglHelper();
static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

void setup()
{
    Serial.begin(115200);
    Serial.println("T-Track");
    Serial.printf("PSRAM: %d MB\r\nflash size : %d MB\r\n", ESP.getPsramSize() / 1024 / 1024, ESP.getFlashChipSize() / 1024 / 1024);

#if !ARDUINO_USB_CDC_ON_BOOT
    Serial.end();
#endif

    // If there is power control on other board then change the pin number
    //T-Track must set poweron to high
    pinMode(POWER_ON_PIN, OUTPUT);
    digitalWrite(POWER_ON_PIN, HIGH);

    lvglHelper();

    Wire.begin(TRACK_QWIIC_SDA, TRACK_QWIIC_SCL);


    // default settings
    sensorOnline_BME280 = bme.begin(0x77, &Wire);
    if (sensorOnline_BME280) {
        bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_X2   );
    }

    sensorOnline_MPU9250 = IMU.begin();

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(cont, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_size(cont, lv_disp_get_physical_hor_res(NULL), lv_disp_get_physical_ver_res(NULL));
    lv_obj_remove_style(cont, 0, LV_PART_SCROLLBAR);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);


    sensor_label = lv_label_create(cont);
    lv_obj_set_style_text_color(sensor_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(sensor_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_label_set_text(sensor_label, "No Sensor detecte!");
}

void loop()
{
    char buf[256] = {0};

    if (sensorOnline_BME280) {
        if ( millis() > measurementInterval ) {
            measurementInterval = millis() + 1000;

            float temp = bme.readTemperature();
            float press = bme.readPressure() / 100.0F;
            float humi = bme.readHumidity();


            snprintf(buf, 256, "Temperature:%.2f\n\nPressure:%.2f\n\nHumidity:%.2f", temp, press, humi);

            lv_label_set_text(sensor_label, buf);

        }
    }

    if (sensorOnline_MPU9250) {
        if ( millis() > measurementInterval ) {
            measurementInterval = millis() + 200;
            // read the sensor
            IMU.readSensor();

            // display the data
            // Serial.print(IMU.getAccelX_mss(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getAccelY_mss(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getAccelZ_mss(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getGyroX_rads(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getGyroY_rads(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getGyroZ_rads(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getMagX_uT(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getMagY_uT(), 6);
            // Serial.print("\t");
            // Serial.print(IMU.getMagZ_uT(), 6);
            // Serial.print("\t");
            // Serial.println(IMU.getTemperature_C(), 6);

            //Just show mag value
            snprintf(buf, 256, "Temperature:%.2f\nMagX:%.2f\nMagY:%.2f\nMagZ%.2f", IMU.getTemperature_C(), IMU.getMagX_uT(), IMU.getMagY_uT(), IMU.getMagZ_uT());
            lv_label_set_text(sensor_label, buf);
        }
    }

    lv_timer_handler();
    delay(1);
}


static void lvglHelper()
{
    static lv_disp_drv_t disp_drv;
    static lv_indev_drv_t indev_drv;
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t *buf;
    JD9613_Init();

    lv_init();

    size_t lv_size = sizeof(lv_color_t) * TRACK_DISP_WIDTH * TRACK_DISP_HEIGHT;
    buf = (lv_color_t *)ps_malloc(lv_size);
    assert(buf);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, lv_size);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TRACK_DISP_WIDTH;
    disp_drv.ver_res = TRACK_DISP_HEIGHT;
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

}


static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lcd_PushColors_SoftRotation(area->x1, area->y1, w, h, (uint16_t *)&color_p->full, 2); // Horizontal display
    lv_disp_flush_ready(disp);
}

