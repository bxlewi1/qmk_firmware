/* Copyright 2026 Q13 Pro autodetect keymap contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Keep Keychron's stock VIA-enabled four-layer keymap as the source of truth. */
#include "../via/keymap.c"

#include "dip_switch.h"
#include "timer.h"
#include "transport.h"
#include "usb_device_state.h"

#define USB_RECONNECT_RESET_DELAY 250

static os_variant_t detected_os = OS_UNSURE;

static volatile usb_configure_state_t previous_usb_state          = USB_DEVICE_STATE_NO_INIT;
static volatile bool                  usb_was_configured          = false;
static volatile bool                  usb_disconnect_seen         = false;
static volatile bool                  usb_reconnect_reset_pending = false;
static volatile fast_timer_t          usb_reconnect_configured_at = 0;

static void select_base_layer(uint8_t layer) {
    if (get_highest_layer(default_layer_state) != layer) {
        default_layer_set(1UL << layer);
    }
}

static void apply_detected_usb_layout(void) {
    if (get_transport() != TRANSPORT_USB) {
        return;
    }

    switch (detected_os) {
        case OS_MACOS:
        case OS_IOS:
            select_base_layer(MAC_BASE);
            break;
        case OS_WINDOWS:
        case OS_LINUX:
            select_base_layer(WIN_BASE);
            break;
        case OS_UNSURE:
            /* Keep the physical Mac/Windows switch as the safe fallback. */
            break;
    }
}

bool process_detected_host_os_user(os_variant_t os) {
    detected_os = os;
    apply_detected_usb_layout();
    return true;
}

void notify_usb_device_state_change_user(struct usb_device_state usb_device_state) {
    usb_configure_state_t current_usb_state = usb_device_state.configure_state;

    if (current_usb_state == previous_usb_state) {
        return;
    }

    previous_usb_state = current_usb_state;

    if (current_usb_state == USB_DEVICE_STATE_CONFIGURED) {
        if (usb_disconnect_seen) {
            /* Reset only after the replacement host has configured USB. */
            usb_disconnect_seen         = false;
            usb_reconnect_reset_pending = true;
            usb_reconnect_configured_at = timer_read_fast();
        }
        usb_was_configured = true;
    } else if (current_usb_state <= USB_DEVICE_STATE_INIT && usb_was_configured) {
        usb_disconnect_seen         = true;
        usb_reconnect_reset_pending = false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && keycode == KC_ESC && (layer_state_is(MAC_FN) || layer_state_is(WIN_FN))) {
        /* Fn+Esc enters STM32 DFU without disconnecting USB power or data. */
        reset_keyboard();
        return false;
    }

    return true;
}

void matrix_scan_user(void) {
    static transport_t previous_transport = TRANSPORT_NONE;
    transport_t        current_transport  = get_transport();

    if (usb_reconnect_reset_pending && usb_device_state_get_configure_state() == USB_DEVICE_STATE_CONFIGURED && timer_elapsed_fast(usb_reconnect_configured_at) >= USB_RECONNECT_RESET_DELAY) {
        usb_reconnect_reset_pending = false;
        soft_reset_keyboard();
        return;
    }

    if (current_transport != previous_transport) {
        previous_transport = current_transport;

        if (current_transport != TRANSPORT_USB || detected_os == OS_UNSURE) {
            /* Bluetooth does not expose its host's USB fingerprint to QMK. */
            dip_switch_read(true);
        }
    }

    /* Reassert auto-selection if the physical switch moves while on USB. */
    apply_detected_usb_layout();
}
