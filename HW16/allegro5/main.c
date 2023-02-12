/* nuklear - 1.32.0 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <allegro5/allegro.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_ALLEGRO5_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_allegro5.h"


#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])


/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the defines */
/*#define INCLUDE_ALL */
/*#define INCLUDE_STYLE */
/*#define INCLUDE_CALCULATOR */
/*#define INCLUDE_CANVAS */
/*#define INCLUDE_OVERVIEW */
/*#define INCLUDE_NODE_EDITOR */

/*
#ifdef INCLUDE_ALL
  #define INCLUDE_STYLE
  #define INCLUDE_CALCULATOR
  #define INCLUDE_CANVAS
  #define INCLUDE_OVERVIEW
  #define INCLUDE_NODE_EDITOR
#endif

#ifdef INCLUDE_STYLE
  #include "../../demo/common/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
  #include "../../demo/common/calculator.c"
#endif
#ifdef INCLUDE_CANVAS
  #include "../../demo/common/canvas.c"
#endif
#ifdef INCLUDE_OVERVIEW
  #include "../../demo/common/overview.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
  #include "../../demo/common/node_editor.c"
#endif

*/

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
int
main (void)
{
  /* Platform */
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  NkAllegro5Font *font;
  struct nk_context *ctx;

  if (!al_init ())
    {
      fprintf (stdout, "failed to initialize allegro5!\n");
      exit (1);
    }
  al_init_image_addon ();
  al_install_mouse ();
  al_set_mouse_wheel_precision (150);
  al_install_keyboard ();

  al_set_new_display_flags (ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE |
			    ALLEGRO_OPENGL);
  al_set_new_display_option (ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option (ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  display = al_create_display (WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!display)
    {
      fprintf (stdout, "failed to create display!\n");
      exit (1);
    }

  event_queue = al_create_event_queue ();
  if (!event_queue)
    {
      fprintf (stdout, "failed to create event_queue!\n");
      al_destroy_display (display);
      exit (1);
    }

  al_register_event_source (event_queue,
			    al_get_display_event_source (display));
  al_register_event_source (event_queue, al_get_mouse_event_source ());
  al_register_event_source (event_queue, al_get_keyboard_event_source ());

  if (!sap_load_bitmap ())
    return EXIT_FAILURE;

  bool running = true;

  font = nk_allegro5_font_create_from_file ("Roboto-Regular.ttf", 12, 0);

  ctx = nk_allegro5_init (font, display, WINDOW_WIDTH, WINDOW_HEIGHT);

  /* style.c */
#ifdef INCLUDE_STYLE
  /* ease regression testing during Nuklear release process; not needed for anything else */
#ifdef STYLE_WHITE
  set_style (ctx, THEME_WHITE);
#elif defined(STYLE_RED)
  set_style (ctx, THEME_RED);
#elif defined(STYLE_BLUE)
  set_style (ctx, THEME_BLUE);
#elif defined(STYLE_DARK)
  set_style (ctx, THEME_DARK);
#endif
#endif

  while (running)
    {
      bool get_event;
      ALLEGRO_EVENT ev;
      ALLEGRO_TIMEOUT timeout;
      al_init_timeout (&timeout, 0.06);

      get_event = al_wait_for_event_until (event_queue, &ev, &timeout);

      if (get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
	{
	  running = false;
	  break;
	}

      /* Very Important: Always do nk_input_begin / nk_input_end even if
         there are no events, otherwise internal nuklear state gets messed up */
      nk_input_begin (ctx);
      if (get_event)
	{
	  while (get_event)
	    {
	      nk_allegro5_handle_event (&ev);
	      get_event = al_get_next_event (event_queue, &ev);
	    }
	}
      nk_input_end (ctx);

      /* GUI */
      if (nk_begin
	  (ctx, "Menu", nk_rect (0, 0, WINDOW_WIDTH, SAP_HEIGHT_MENU), 0))
	{

	  nk_layout_row_static (ctx, 20, 80, 4);
	  if (nk_button_label (ctx, "START"))
	    sap_init ();

	  if (nk_option_label (ctx, "easy", sap_op == SAP_EASY))
	    sap_op = SAP_EASY;
	  if (nk_option_label (ctx, "middle", sap_op == SAP_MIDDLE))
	    sap_op = SAP_MIDDLE;
	  if (nk_option_label (ctx, "hard", sap_op == SAP_HARD))
	    sap_op = SAP_HARD;
	}
      nk_end (ctx);

      /* Draw */
      al_clear_to_color (al_map_rgba_f (0, 0, 0, 1));
      /* IMPORTANT: `nk_allegro5_render` changes the target backbuffer
         to the display set at initialization and does not restore it.
         Change it if you want to draw somewhere else. */
      nk_allegro5_render ();
      sap_render ();
      al_flip_display ();
    }

  sap_destroy ();
  nk_allegro5_font_del (font);
  nk_allegro5_shutdown ();
  al_destroy_display (display);
  al_destroy_event_queue (event_queue);
  return 0;
}
