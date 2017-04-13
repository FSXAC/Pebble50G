#pragma once
#include <pebble.h>

static void main_window_load(Window *w);
static void main_window_unload(Window *w);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();
static void init();
static void deinit();