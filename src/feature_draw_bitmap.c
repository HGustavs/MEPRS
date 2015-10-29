
#include "pebble.h"

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;
static GBitmap *t_image[20];

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

	// Define bitmap
  s_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);

	// Define tiles
	for(int i=0;i<10;i++){
			t_image[i] = gbitmap_create_as_sub_bitmap(s_image, GRect(t*12, 0, 12, 12));			
	}	
}

static void main_window_unload(Window *window) {
	// Deallocate tiles
	for(int i=0;i<10;i++){
			 gbitmap_destroy(t_image[i]);
	}
	
	// Deallocate bitmap
 	gbitmap_destroy(s_image);
	
	// Destroy layer
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
