#include "AllegroEngine.h"
#include <stdio.h>
#include <string>

AllegroEngine::AllegroEngine(): timer_fps(0), event_queue(0), root(0), font(0), quit(false), minimized(false)
  {
  }

AllegroEngine::~AllegroEngine()
  {
  }

void AllegroEngine::setFPS(map<int,bool> errormap, int fps)
  {
  timer_fps = al_create_timer(1.0/fps);
  if (!timer_fps) errormap.insert(pair<int,bool>(E_TIMER,true)); //Did the timer not be created?
  }

bool AllegroEngine::Init(int winwidth,int winheight)
  {
  map<int,bool> errormap;
  //
  if (!al_init()) errormap.insert(pair<int,bool>(E_ALLEGRO,true)); //Did Allegro fail to open?
  //
  root = al_create_display(winwidth, winheight); 
  if (!root) errormap.insert(pair<int,bool>(E_DISPLAY,true)); //Did the root display fail to be created?
  //
  al_init_font_addon(); // initialize the font addon
  al_init_ttf_addon();// initialize the ttf (True Type Font) addon
  font = al_load_ttf_font("malgun.ttf",72,0 ); 
  if (!font) errormap.insert(pair<int,bool>(E_FONT,true)); //Did the font fail to load?
  //
  setFPS(errormap, 120);
  //
  if (!al_install_mouse()) errormap.insert(pair<int,bool>(E_MOUSE,true)); //No mouse?
  if (!al_install_keyboard()) errormap.insert(pair<int,bool>(E_KEYBOARD,true)); //No mouse?
  //
  event_queue = al_create_event_queue();
  if (!event_queue) errormap.insert(pair<int,bool>(E_EVENTQUEUE,true)); //Did the event queue not appear?
  //
  al_register_event_source(event_queue, al_get_display_event_source(root)); //register display as event source
  al_register_event_source(event_queue, al_get_timer_event_source(timer_fps)); //register timer as event source
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_mouse_event_source());
  //
  bool errors = EngineInit(errormap);
  return errors || errormap.size()==0?true:false;
  }

void AllegroEngine::Start()
  {
  al_start_timer(timer_fps);
  quit=false;
  while(!quit)
    Run();
  }

void AllegroEngine::Run()
  {
  ALLEGRO_EVENT ev;

  while (!quit) {
    al_wait_for_event(event_queue, &ev);
    switch (ev.type) 
      {
      /* ALLEGRO_EVENT_KEY_DOWN - a keyboard key was pressed.
      * The three keyboard event fields we use here are:
      *
      * keycode -- an integer constant representing the key, e.g.
        *             AL_KEY_ESCAPE;
        *
        * unichar -- the Unicode character being typed, if any.  This can
        *             depend on the modifier keys and previous keys that were
        *             pressed, e.g. for accents.
          *
          * modifiers -- a bitmask containing the state of Shift/Ctrl/Alt, etc.
          *             keys.
          */
    case ALLEGRO_EVENT_KEY_DOWN:
      if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        quit = true;
      KeyDown(ev.keyboard.keycode, ev.keyboard.unichar, ev.keyboard.modifiers);
      break;

        /* ALLEGRO_EVENT_KEY_REPEAT - a keyboard key was held down long enough to
          * 'repeat'.  This is a useful event if you are working on something
          * that requires typed input.  The repeat rate should be determined
          * by the operating environment the program is running in.
          */
      case ALLEGRO_EVENT_KEY_CHAR:
        KeyDown(ev.keyboard.keycode, ev.keyboard.unichar, ev.keyboard.modifiers, ev.keyboard.repeat);
        break;

        /* ALLEGRO_EVENT_KEY_UP - a keyboard key was released.
        * Note that the unichar field is unused for this event.
          */
      case ALLEGRO_EVENT_KEY_UP:
        KeyUp(ev.keyboard.keycode, ev.keyboard.unichar, ev.keyboard.modifiers);
        break;

        /* ALLEGRO_EVENT_MOUSE_AXES - at least one mouse axis changed value.
        * The 'z' axis is for the scroll wheel.  We also have a fourth 'w'
        * axis for mice with two scroll wheels.
          */
         case ALLEGRO_EVENT_MOUSE_AXES:
            MouseMoved(ev.mouse.x, ev.mouse.y, ev.mouse.z, ev.mouse.dx, ev.mouse.dy, ev.mouse.dz);
            break;
 
         /* ALLEGRO_EVENT_MOUSE_BUTTON_UP - a mouse button was pressed. 
          * The axis fields are also valid for this event.
          */
         case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            MouseButtonDown(ev.mouse.button, ev.mouse.x, ev.mouse.y, ev.mouse.z, ev.mouse.dx, ev.mouse.dy, ev.mouse.dz);
            break;
 
         /* ALLEGRO_EVENT_MOUSE_BUTTON_UP - a mouse button was released.
          * The axis fields are also valid for this event.
          */
         case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            MouseButtonUp(ev.mouse.button, ev.mouse.x, ev.mouse.y, ev.mouse.z, ev.mouse.dx, ev.mouse.dy, ev.mouse.dz);
            break;
 
         /* ALLEGRO_EVENT_TIMER - a timer 'ticked'.
          * The `source' field in the event structure tells us which timer
          * went off, and the `count' field tells us the timer's counter
          * value at the time that the event was generated.  It's not
          * redundant, because although you can query the timer for its
          * counter value, that value might have changed by the time you got
          * around to processing this event.
          */
         case ALLEGRO_EVENT_TIMER:
            HandleTimers(ev.timer.source, ev.timer.count);
            break;

         /* ALLEGRO_EVENT_DISPLAY_CLOSE - the window close button was pressed.
          */
         case ALLEGRO_EVENT_DISPLAY_CLOSE:
           quit = true;
            return;
 
         /*case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
            log_general("Switch In");
            break;
 
         case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
            log_general("Switch Out");
            break;*/
 
         /* We received an event of some type we don't know about.
          * Just ignore it.
          */
         default:
            break;
      }
   }
  }

void AllegroEngine::DoRender()
  {
  al_set_target_backbuffer(root);
  al_clear_to_color(al_map_rgb(0,0,0));
  Render(root);
  al_flip_display();
  }

void AllegroEngine::End()
  {
  }