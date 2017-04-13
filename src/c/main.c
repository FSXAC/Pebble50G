//#include <pebble.h>
#include "main.h"

// window global variable
static Window *s_main_window;

// use TextLayer to show text
static TextLayer *s_time_layer;

static void main_window_load(Window *w) {
    // get information about the window
    Layer *window_layer = window_get_root_layer(w);
    GRect bounds = layer_get_bounds(window_layer);
    
    // create textlayer with specific bounds
    s_time_layer = text_layer_create(GRect(0, 10, bounds.size.w, 50));
    
    // imrpove the layout to be more like a watchface
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "--:--");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_09));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
    
    // add as child layer to main window
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *w) {
    // destroy elements in window
    // destroy textlayer
    text_layer_destroy(s_time_layer);
}

// subscribe to a function to tell the time
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

// updates the text with time
static void update_time() {
    // get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    // write current hours and minutes to a buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    
    // display the time to textlayer
    text_layer_set_text(s_time_layer, s_buffer);
}

static void init() {
    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    
    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);
    
    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
    // make sure the time is dispayed from the start
    update_time();
}

static void deinit() {
    // destry window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}