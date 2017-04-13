//#include <pebble.h>
#include "main.h"

// window global variable
static Window *s_main_window;

// use TextLayer to show text
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_backg_layer;

// global custom fonts
static GFont s_time_font;

static void main_window_load(Window *w) {
    // get information about the window
    Layer *window_layer = window_get_root_layer(w);
    GRect bounds = layer_get_bounds(window_layer);
    
    // create textlayer with specific bounds
    s_time_layer = text_layer_create(GRect(0, 17, bounds.size.w-50, 40));
    s_date_layer = text_layer_create(GRect(0, 17, bounds.size.w-2, 40));
    s_backg_layer = text_layer_create(GRect(2, 0, bounds.size.w, bounds.size.h-4));
    
    // use custom fonts
    // create GFont
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINIFONT_16));
    
    // assign fonts
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_font(s_date_layer, s_time_font);
    text_layer_set_font(s_backg_layer, s_time_font);
    
    // imrpove the layout to be more like a watchface
    // time
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "--:--");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
    // date
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text(s_date_layer, "---:--");
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
    
    // background text
    text_layer_set_background_color(s_backg_layer, GColorClear);
    text_layer_set_text_color(s_backg_layer, GColorBlack);
    text_layer_set_text(s_backg_layer, "RAD XYZ R= 'X'\n{HOME}\n7:\n6:\n5:\n4:\n3:\n2:\n1:");
    text_layer_set_text_alignment(s_backg_layer, GTextAlignmentLeft);
    
    // add as child layer to main window
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_backg_layer));
}

static void main_window_unload(Window *w) {
    // destroy elements in window
    // destroy textlayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_backg_layer);
    
    // destroy font
    fonts_unload_custom_font(s_time_font);
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
    static char date_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    
    // display the time to textlayer
    text_layer_set_text(s_time_layer, s_buffer);
    
    // display date
    strftime(date_buffer, sizeof(date_buffer), "%b:%d", tick_time);
    
    // display the date to date textlayer
    text_layer_set_text(s_date_layer, date_buffer);
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