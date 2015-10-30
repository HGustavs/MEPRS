
#include "pebble.h"

#define PERSIST_KEY_SCORE 771

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;
static GBitmap *t_image[64];
static unsigned char tilemap[576];
static unsigned char hiscore[25];
static unsigned char score[5];

// Player coordinate and player direction
static int px;
static int py;
static int pdx;
static int pdy;
static int state;
int level;

static int comparescore()
{
		int scored=(score[0])+(10*score[1])+(100*score[2])+(1000*score[3])+(1000*score[4]);
		int scorel=(hiscore[(level*5)+0])+(10*hiscore[(level*5)+1])+(100*hiscore[(level*5)+2])+(1000*hiscore[(level*5)+3])+(1000*hiscore[(level*5)+4]);	
		return (scored>scorel);
}

// Clear playfield and init scoreboard
static void initGameBoard()
{
	level=0;
	
	// Read High Score.... from persistent storage (or clear it)
	//	if (persist_exists(PERSIST_KEY_SCORE)) {
	if (0) {	
			persist_read_data(PERSIST_KEY_SCORE, hiscore, 25);
	}else{
			for(int i=0;i<25;i++){
					hiscore[i]=0;
			}			
	}

	// Clear Score
	for(int i=0;i<5;i++){
			score[i]=0;
	}	
	
	// Assign some data to tile map
	for(int i=0;i<576;i++){
			tilemap[i]=0;		
	}

	// Clear all edges of tilemap
	for(int i=0;i<24;i++){
			tilemap[i]=10;
			tilemap[(23*24)+i]=10;
			tilemap[i*24]=10;
			tilemap[(i*24)+23]=10;
	}
	
	// Move player to center of screen
	px=12;
	py=12;
	pdx=1;
	pdy=0;

	// Randomize mines
	int tx,ty;
	for(int i=0;i<70;i++){
			tx=1+(rand()%22);
			ty=1+(rand()%22);
			tilemap[(tx*24)+ty]=100;
	}

	// Secure Player Position
	tilemap[(12*24)+12]=10;

	// Compute Numbers
	for(int i=1;i<23;i++){
		for(int j=1;j<23;j++){
			int val=0;
			for(int k=-1;k<=1;k++){
				for(int l=-1;l<=1;l++){
						if(tilemap[((j+l)*24)+i+k]>=100){
								val++;
						}
				}
			}
			tilemap[(j*24)+i]+=val;	
		}
	}
	tilemap[(12*24)+12]=10;

}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
		if(state==1){
				// Only move if allowed
				if(((px+pdx)<23)&&((px+pdx)>0)) px+=pdx;
				if(((py+pdy)<23)&&((py+pdy)>0)) py+=pdy;

				// Check if game over!
				if(tilemap[px+(py*24)]>=100){
						// game over!
						state=2;
					
						// Update hiscores
					
						// Write score
						persist_write_data(PERSIST_KEY_SCORE, &hiscore, 25);
				}else{
						// Add score - do not count previously added tiles
						int tiles=tilemap[px+(py*24)];
						if(tiles<10){
								score[0]+=tiles+1;
								for(int i=0;i<5;i++){
										if(score[i]>9){
												score[i+1]++;
												score[i]-=10;
										}
								}
						}
						// mark tile as safe.
						tilemap[px+(py*24)]=10;
				}
		}else if(state==2){
				state=1;
				initGameBoard();
		}
	
		// Redraw Graphics
    layer_mark_dirty(s_image_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
		if(state==1){
				if(pdx==1){
						pdx=0;
						pdy=1;
				}else if(pdx==-1){
						pdx=0;
						pdy=-1;
				}else if(pdy==1){
						pdx=-1;
						pdy=0;
				}else if(pdy==-1){
						pdx=1;
						pdy=0;
				}
		}
    layer_mark_dirty(s_image_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
		if(state==1){
			if(pdx==1){
						pdx=0;
						pdy=-1;
				}else if(pdx==-1){
						pdx=0;
						pdy=1;
				}else if(pdy==1){
						pdx=1;
						pdy=0;
				}else if(pdy==-1){
						pdx=-1;
						pdy=0;
				}
		}
    layer_mark_dirty(s_image_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void layer_update_callback(Layer *layer, GContext* ctx) {
	
	if(state==0){
			// Draw Logo and High Scores
	}else if(state>0){
			// Update Tilemap
			int cx;
			int cy;
			int ccx;
			int tileno;

			cx=px-6;
			cy=py-6;
			if(cx<0) cx=0;
			if(cx>12) cx=12;
			if(cy<0) cy=0;
			if(cy>12) cy=12;	

			// Y coordinate first, loop over x coordinate
			for(int j=0;j<12;j++){
				ccx=cx;
				for(int i=0;i<12;i++){
						tileno=tilemap[(cy*24)+ccx];
						if(state==2){
								if(tileno>=100) tileno=11;
								else if(tileno==10) tileno=10;
								else tileno=0;
						}
						if(tileno>100) tileno=tileno-100;
						if((ccx==px)&&(cy==py)) tileno=11;
						if((ccx==(px+pdx))&&(cy==(py+pdy))) tileno+=12;
						graphics_draw_bitmap_in_rect(ctx, t_image[tileno], GRect(i*12,j*12, 12, 12));			
						ccx++;
				}
				cy++;
			}

			// Clear to the left of score
			for(int i=0;i<3;i++){
					graphics_draw_bitmap_in_rect(ctx, t_image[34], GRect((i*14),144, 14, 24));
					graphics_draw_bitmap_in_rect(ctx, t_image[34], GRect((i*14)+108,144, 14, 24));
			}

			// Update Score (slight overdraw to accomplish centering...)
			for(int i=0;i<5;i++){
					graphics_draw_bitmap_in_rect(ctx, t_image[score[i]+24], GRect(94-(i*14),144, 14, 24));
			}		
	}
}

//---------------------------------------------------------------------------------//
// main_window_load
//---------------------------------------------------------------------------------//
// Called when window is defined (i.e. once per application start)
//---------------------------------------------------------------------------------//

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
			if(i<3) t_image[i+34] = gbitmap_create_as_sub_bitmap(s_image, GRect(i*14, 48, 14, 24));
	}
	
	// Prepare for new game
	state=1;
	initGameBoard();

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
  window_set_click_config_provider(s_main_window, click_config_provider);
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
