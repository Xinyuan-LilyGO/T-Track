#include "gui.h"

LV_FONT_DECLARE(alibaba_font_48);
LV_FONT_DECLARE(alibaba_font_18);

static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *week_label;
static lv_obj_t *month_label;
static bool colon;
const char *week_char[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *month_char[] = LV_CALENDAR_DEFAULT_MONTH_NAMES;

static void update_date(lv_timer_t *e)
{

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        lv_label_set_text_fmt(time_label, "%02d%s%02d", timeinfo.tm_hour, colon != 0 ? "#ffffff :#" : "#000000 :#", timeinfo.tm_min);
        colon = !colon;
        lv_label_set_text_fmt(date_label, "%d", timeinfo.tm_mday);
        lv_label_set_text_fmt(week_label, "%s", week_char[timeinfo.tm_wday]);
        lv_label_set_text_fmt(month_label, "%s", month_char[timeinfo.tm_mon]);
    }
}

void gui_init(void)
{
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_size(cont, 294, 126);
    lv_obj_remove_style(cont, 0, LV_PART_SCROLLBAR);

    lv_obj_t *time_cont = lv_obj_create(cont);
    lv_obj_set_size(time_cont, 170, 90);
    lv_obj_align(time_cont, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(time_cont, lv_color_white(), 0);
    lv_obj_set_style_shadow_color(time_cont, lv_color_white(), 0);
    lv_obj_set_style_shadow_width(time_cont, 20, 0);
    // lv_obj_set_style_sha

    time_label = lv_label_create(time_cont);
    lv_obj_set_style_text_font(time_label, &alibaba_font_48, 0);
    lv_label_set_recolor(time_label, 1);
    lv_obj_center(time_label);
    lv_label_set_text(time_label, "12:34");

    date_label = lv_label_create(cont);
    lv_obj_align(date_label, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_color(date_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(date_label, &alibaba_font_48, 0);
    lv_label_set_text(date_label, "30");

    week_label = lv_label_create(cont);
    lv_obj_set_width(week_label, 100);
    lv_label_set_long_mode(week_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(week_label, LV_ALIGN_TOP_RIGHT, 10, 5);
    lv_obj_set_style_text_color(week_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(week_label, &alibaba_font_18, 0);
    lv_obj_set_style_text_align(week_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(week_label, "Thursday");

    month_label = lv_label_create(cont);
    lv_obj_set_width(month_label, 100);
    lv_label_set_long_mode(month_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(month_label, LV_ALIGN_BOTTOM_RIGHT, 10, -5);
    lv_obj_set_style_text_color(month_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(month_label, &alibaba_font_18, 0);
    lv_obj_set_style_text_align(month_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(month_label, "February");

    lv_timer_create(update_date, 500, NULL);
}