#include "scroll_filter.h"

void scroll_filter_process(struct zmk_pointing_sensor_event *event,
                           const struct scroll_filter_config *cfg) {

    /* スクロールモード中だけ呼び出す想定
       X の動きが deadzone 以内なら横入力を 0 にする */
    if (event->dx < cfg->deadzone_x && event->dx > -cfg->deadzone_x) {
        event->dx = 0;
    }
}
