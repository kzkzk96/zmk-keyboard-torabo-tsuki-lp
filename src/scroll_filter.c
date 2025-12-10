// src/scroll_filter.c
#include <device.h>
#include <zephyr.h>
#include <zmk/event_manager.h>
#include <zmk/events/pointing_motion.h>
#include <zmk/keys.h>
#include <zmk/led.h>
#include <logging/log.h>
#include <zmk/input.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/position_changed.h>
#include <zmk/events/pointing_motion.h>
#include <settings/settings.h>
#include <zmk/behavior.h>
#include <zmk/matrix.h>
#include <zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/* Deadzone value (pixels). 要求: デッドゾーン = 8 */
#define SCROLL_DEADZONE_X 8

/* Helper to check whether layer 5 is active.
   Implementation: use zmk_keymap_layer_active() if available.
   If not available on your ZMK, CI でエラー出ます -> その場合は教えてください。 */
#ifdef CONFIG_ZMK
#include <zmk/keymap.h>
#endif

static bool layer5_active(void) {
#ifdef CONFIG_ZMK
    /* zmk_keymap_layer_active(layer) returns true if active (API exists in many zmk versions) */
    return zmk_keymap_layer_active(5);
#else
    /* fallback: assume not active */
    return false;
#endif
}

/* This function receives pointing motion events and filters X component
   only when layer5_active() == true. Y component is always passed through.
   If abs(x) < SCROLL_DEADZONE_X and layer5 active => x = 0 */
void scroll_filter_process(struct zmk_pointing_motion_evt *evt) {
    if (!evt) return;

    if (layer5_active()) {
        int32_t x = evt->dx;
        /* apply deadzone on X */
        if (x > -SCROLL_DEADZONE_X && x < SCROLL_DEADZONE_X) {
            evt->dx = 0;
        }
        /* Y left unchanged */
    }
}

/* Event listener to capture pointing motion events and apply filter.
   Subscribe to zmk pointing motion events. */
static int scroll_filter_event_listener(const zmk_event_t *eh) {
    if (!eh) {
        return -1;
    }

    if (is_zmk_pointing_motion_event(eh)) {
        struct zmk_pointing_motion_evt *pm = as_zmk_pointing_motion_event(eh);
        /* modify in-place */
        scroll_filter_process(pm);
    }
    return 0;
}

/* Register the listener */
static struct zmk_event_listener scroll_filter_listener = {
    .name = "scroll_filter_listener",
    .callback = scroll_filter_event_listener,
};

ZMK_LISTENER(scroll_filter_listener);
ZMK_SUBSCRIPTION(scroll_filter_listener, zmk_pointing_motion_state_changed);
ZMK_SUBSCRIPTION(scroll_filter_listener, zmk_pointing_motion);
