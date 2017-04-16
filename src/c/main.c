//#include <pebble.h>
#include "main.h"

// window global variable
static Window *s_main_window;

// use TextLayer to show text
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_backg_layer;

// global custom fonts
static GFont s_minifont;

// global bitmap image
static BitmapLayer *s_topbar_layer;
static GBitmap *s_topbar_bitmap;

// battery indicator
static int s_battery_level;
static Layer *s_battery_layer;

// heart rate indicator
static TextLayer *s_hrm_layer;

static void main_window_load(Window *w) {
    // get information about the window
    Layer *window_layer = window_get_root_layer(w);
    GRect bounds = layer_get_bounds(window_layer);
    
    // create custom GFont
    s_minifont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINIFONT_16));
    
    /*===[BITMAPS]===*/
    // create bitmaps
    s_topbar_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TOPBAR);
    
    // create bitmap layer to display the bitmap
    s_topbar_layer = bitmap_layer_create(GRect(0, 38, bounds.size.w, 1));
    
    // set bitmap onto the layer
    bitmap_layer_set_bitmap(s_topbar_layer, s_topbar_bitmap);
    
    // add bitmap layers as child layer to main window
    layer_add_child(window_layer, bitmap_layer_get_layer(s_topbar_layer));
    
    /*===[MAIN TIME]====*/
    // create textlayer with specific bounds
    s_time_layer = text_layer_create(GRect(0, 17, bounds.size.w-50, 40));
    
    // assign fonts
    text_layer_set_font(s_time_layer, s_minifont);
    
    // improve the layout to be more like a watchface
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "--:--");
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
    
    // add text layers as child layer to main window
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    
    /*===[DATE LAYER]===*/
    s_date_layer = text_layer_create(GRect(0, 17, bounds.size.w-2, 40));
    text_layer_set_font(s_date_layer, s_minifont);\
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_text(s_date_layer, "---:--");
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
    
    /*===[BACKGROUND TEXT]===*/
    s_backg_layer = text_layer_create(GRect(2, 0, bounds.size.w, bounds.size.h-4));
    text_layer_set_font(s_backg_layer, s_minifont);
    text_layer_set_background_color(s_backg_layer, GColorClear);
    text_layer_set_text_color(s_backg_layer, GColorBlack);
    text_layer_set_text(s_backg_layer, "RAD XYZ BIN R= 'X'\n{HOME}\n7:\n6:\n5:\n4:\n3:\n2:\n1:");
    text_layer_set_text_alignment(s_backg_layer, GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(s_backg_layer));
    
    /*===[HEARTRATE MONITOR]===*/
    s_hrm_layer = text_layer_create(GRect(20, 128, bounds.size.w-22, 26));
    text_layer_set_font(s_hrm_layer, s_minifont);
    text_layer_set_background_color(s_hrm_layer, GColorClear);
    text_layer_set_text_color(s_hrm_layer, GColorBlack);
    text_layer_set_text(s_hrm_layer, "HR=---");
    text_layer_set_text_alignment(s_hrm_layer, GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(s_hrm_layer));
    
    /* ===[BATTERY LEVEL]===*/
    // create battery meter layer
    s_battery_layer = layer_create(GRect(2, 150, bounds.size.w-4, 16));
    layer_set_update_proc(s_battery_layer, battery_update_proc);
    
    // add battery layer to window
    layer_add_child(window_layer, s_battery_layer);
}

static void main_window_unload(Window *w) {
    // destroy elements in window
    // destroy textlayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_backg_layer);
    
    // destroy font
    fonts_unload_custom_font(s_minifont);
    
    // destroy bitmap stuff
    gbitmap_destroy(s_topbar_bitmap);
    bitmap_layer_destroy(s_topbar_layer);
    
    // destroy battery stuff
    layer_destroy(s_battery_layer);
    
    // destroy heart rate stuff
    text_layer_destroy(s_hrm_layer);
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

// handler for battery subscription
static void battery_callback(BatteryChargeState state) {
    // record new battery level
    s_battery_level = state.charge_percent;
    
    // mark battery layer to be re-rendered
    layer_mark_dirty(s_battery_layer);
}

// layer update for battery
static void battery_update_proc(Layer *layer, GContext *ctx) {
    // draw the battery indication
    GRect bounds = layer_get_bounds(layer);
    
    // find the width of battery bar
    int width = s_battery_level * 140 / 100;
    
    // draw the graphics
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

// handler for health / heartrate subscription
static void prv_on_health_data(HealthEventType type, void *context) {
    // queue if update was from heart rate monitor
    if (type == HealthEventHeartRateUpdate) {
        HealthValue value = health_service_peek_current_value(HealthMetricHeartRateBPM);
        
        // display the value
        static char s_hrm_buffer[8];
        snprintf(s_hrm_buffer, sizeof(s_hrm_buffer), "HR=%d", (uint8_t)value);
        text_layer_set_text(s_hrm_layer, s_hrm_buffer);
    }
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
    
    // register with battery level updates
    battery_state_service_subscribe(battery_callback);
    
    // register subscription with heart rate updates
    health_service_events_subscribe(prv_on_health_data, NULL);
    
    // make sure the time is dispayed from the start
    update_time();
    
    // make sure the battery level is displayed from the start
    battery_callback(battery_state_service_peek());
    
    // make sure battery level is displayed from the start
    prv_on_health_data(HealthEventHeartRateUpdate, 0);
}

static void deinit() {
    // destry window
    window_destroy(s_main_window);
    
    // Reset the heart rate sampling period to automatic
    //health_service_set_heart_rate_sample_period(0);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}