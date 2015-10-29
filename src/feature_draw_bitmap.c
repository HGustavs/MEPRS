
#include "pebble.h"

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;
static GBitmap *t_image[4];

static void layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_draw_bitmap_in_rect(ctx, t_image[0], GRect(30, 30, 12, 12));
  graphics_draw_bitmap_in_rect(ctx, t_image[1], GRect(42, 30, 12, 12));
  graphics_draw_bitmap_in_rect(ctx, t_image[2], GRect(54, 30, 12, 12));
  graphics_draw_bitmap_in_rect(ctx, t_image[3], GRect(66, 85, 12, 12));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);

  s_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);

	// Define tiles
	t_image[0] = gbitmap_create_as_sub_bitmap(s_image, GRect(0, 0, 12, 12));
	t_image[1] = gbitmap_create_as_sub_bitmap(s_image, GRect(12, 0, 12, 12));
	t_image[2] = gbitmap_create_as_sub_bitmap(s_image, GRect(24, 0, 12, 12));
	t_image[3] = gbitmap_create_as_sub_bitmap(s_image, GRect(36, 0, 12, 12));
	
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_image);
  gbitmap_destroy(t_image[0]);
  gbitmap_destroy(t_image[1]);
  gbitmap_destroy(t_image[2]);
  gbitmap_destroy(t_image[3]);
	layer_destroy(s_image_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
