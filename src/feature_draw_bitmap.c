
#include "pebble.h"

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;
static GBitmap *t_image[64];

static void layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_draw_bitmap_in_rect(ctx, t_image[13], GRect(30, 30, 12, 12));
  graphics_draw_bitmap_in_rect(ctx, t_image[7], GRect(42, 30, 12, 12));
  graphics_draw_bitmap_in_rect(ctx, t_image[15], GRect(54, 30, 12, 12));
  graphics_draw_bitmap_in_rect(ctx, t_image[3], GRect(66, 30, 12, 12));

  graphics_draw_bitmap_in_rect(ctx, t_image[25], GRect(100, 100, 14, 21));

}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);

	// Define bitmap
  s_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);

	// Define tiles (2 rows of 12 tiles at 12x12 px)
	for(int i=0;i<12;i++){
			t_image[i] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*12, 0, 12, 12));			
			t_image[i+12] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*12, 12, 12, 12));			
			if(i<10) t_image[i+24] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*14, 24, 14, 21));
	}	
}

static void main_window_unload(Window *window) {
	// Deallocate tiles
	for(int i=0;i<34;i++){
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
