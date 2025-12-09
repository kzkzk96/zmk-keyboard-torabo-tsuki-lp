#pragma once

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zmk/event_manager.h>
#include <zmk/events/pointing_sensor_event.h>

struct scroll_filter_config {
    int deadzone_x;
};

void scroll_filter_process(struct zmk_pointing_sensor_event *event,
                           const struct scroll_filter_config *cfg);
