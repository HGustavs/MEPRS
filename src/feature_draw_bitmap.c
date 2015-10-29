
#include "pebble.h"

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;
static GBitmap *t_image[64];
static unsigned char tilemap[256];
static unsigned char score[6];

static void layer_update_callback(Layer *layer, GContext* ctx) {

	// Update Tilemap
	for(int i=0;i<12;i++){
		for(int j=0;j<12;j++){
  			graphics_draw_bitmap_in_rect(ctx, t_image[tilemap[(j*12)+i]], GRect(i*12,j*12, 12, 12));			
		}
	}
	
	// Update Score
	for(int i=0;i<5;i++){
			graphics_draw_bitmap_in_rect(ctx, t_image[score[i]+24], GRect(i*14,144, 14, 24));
	}

}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);

	// Define bitmap
  s_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);

	// Define tiles (2 rows of 12 tiles at 12x12 px and 1 row of 14x23)
	for(int i=0;i<12;i++){
			t_image[i] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*12, 0, 12, 12));			
			t_image[i+12] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*12, 12, 12, 12));			
			if(i<10) t_image[i+24] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*14, 24, 14, 24));
	}
	
	// Assign some data to tile map
	for(int i=0;i<144;i++){
			tilemap[i]=0;		
	}
	for(int i=0;i<12;i++){
			tilemap[i]=1;
			tilemap[i*12]=2;
	}
	tilemap[78]=11;
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
