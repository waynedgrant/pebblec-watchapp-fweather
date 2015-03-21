// Copyright 2015 Wayne D Grant (www.waynedgrant.com)
// Licensed under the MIT License

#include <pebble.h>
  
#define KEY_LOCATION 0
#define KEY_TEMPERATURE 1

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_location_layer;
static TextLayer *s_temperature_layer;
static TextLayer *s_its_fucking_layer;
static TextLayer *s_whatever_it_is_layer;

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";

  if (clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  s_time_layer = text_layer_create(GRect(0, 0, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  s_location_layer = text_layer_create(GRect(0, 52, 144, 24));
  text_layer_set_background_color(s_location_layer, GColorBlack);
  text_layer_set_text_color(s_location_layer, GColorWhite);
  text_layer_set_font(s_location_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
  text_layer_set_text(s_location_layer, "Fucking Nowhere");
  
  s_temperature_layer = text_layer_create(GRect(0, 73, 144, 38));
  text_layer_set_background_color(s_temperature_layer, GColorBlack);
  text_layer_set_text_color(s_temperature_layer, GColorWhite);
  text_layer_set_font(s_temperature_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentCenter);
  text_layer_set_text(s_temperature_layer, "--\u00B0C");
  
  s_its_fucking_layer = text_layer_create(GRect(0, 104, 144, 35));
  text_layer_set_background_color(s_its_fucking_layer, GColorBlack);
  text_layer_set_text_color(s_its_fucking_layer, GColorWhite);
  text_layer_set_font(s_its_fucking_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_its_fucking_layer, GTextAlignmentCenter);
  text_layer_set_text(s_its_fucking_layer, "IT'S FUCKING");
  
  s_whatever_it_is_layer = text_layer_create(GRect(0, 133, 144, 35));
  text_layer_set_background_color(s_whatever_it_is_layer, GColorBlack);
  text_layer_set_text_color(s_whatever_it_is_layer, GColorWhite);
  text_layer_set_font(s_whatever_it_is_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_whatever_it_is_layer, GTextAlignmentCenter);
  text_layer_set_text(s_whatever_it_is_layer, "LOADING!");
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_location_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temperature_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_its_fucking_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_whatever_it_is_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_location_layer);
  text_layer_destroy(s_temperature_layer);
  text_layer_destroy(s_its_fucking_layer);
  text_layer_destroy(s_whatever_it_is_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  if(tick_time->tm_min % 15 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    dict_write_uint8(iter, 0, 0);

    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {  
  static char location_buffer[32];
  static char temperature_buffer[8];
  static char whatever_it_is_buffer[10];
  static int temperature;
  int weather_received = 0;

  Tuple *t = dict_read_first(iterator);

  while(t != NULL) {
    weather_received = 1;
    switch(t->key) {
      case KEY_LOCATION:
        snprintf(location_buffer, sizeof(location_buffer), "Fucking %s", t->value->cstring);
        break;
      case KEY_TEMPERATURE:
        temperature = (int)t->value->int32;
        snprintf(temperature_buffer, sizeof(temperature_buffer), "%d\u00B0C ?", temperature);
  
        if (temperature < -4) {
          strcpy(whatever_it_is_buffer, "BALTIC!");
        } else if (temperature < 0) {
          strcpy(whatever_it_is_buffer, "FREEZING!");
        } else if (temperature < 9) {
          strcpy(whatever_it_is_buffer, "COLD!");
        } else if (temperature < 19) {
          strcpy(whatever_it_is_buffer, "OKAY!");
        } else if (temperature < 26) {
          strcpy(whatever_it_is_buffer, "WARM!");
        } else if (temperature < 31) {
          strcpy(whatever_it_is_buffer, "HOT!");
        } else {
          strcpy(whatever_it_is_buffer, "ROASTING!");
        }      
        break;
      default:
        break;
    }
    t = dict_read_next(iterator);
  }
  
  if (!weather_received) {
    strcpy(location_buffer, "Fucking Nowhere");
    strcpy(temperature_buffer, "?\u00B0C");
    strcpy(whatever_it_is_buffer, "UNKNOWN!");
  }
  
  text_layer_set_text(s_location_layer, location_buffer);
  text_layer_set_text(s_temperature_layer, temperature_buffer);
  text_layer_set_text(s_whatever_it_is_layer, whatever_it_is_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  
  update_time();
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
