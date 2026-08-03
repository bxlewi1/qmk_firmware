#pragma once

/*
 * Many KVMs keep the keyboard powered while switching its USB data connection
 * between hosts. Reset when USB is reinitialized so OS detection runs again.
 */
#define OS_DETECTION_KEYBOARD_RESET
