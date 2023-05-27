#pragma once

#include "lvgl.h"
#include "time.h"
#include "Arduino.h"

#define MSG_NEW_DATE 1

#define LV_DELAY(x)             \
    do                          \
    {                           \
        uint32_t t = x;         \
        while (t--)             \
        {                       \
            lv_timer_handler(); \
            delay(1);           \
        }                       \
    } while (0);

void gui_init(void);
