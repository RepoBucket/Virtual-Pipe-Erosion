#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <map>
#include <string>

using namespace std;

class AllegroEngine
  {
  private:
    bool quit;
    bool minimized;
    int window_width;
    int window_height;
    
    void Run();
   // void DoUpdate();
    void DoRender();
    void setFPS( map<int,bool> errormap, int fps = 60);

    enum ERRORS {E_ALLEGRO = 1, E_DISPLAY, E_FONT, E_EVENTQUEUE, E_TIMER, E_MOUSE, E_KEYBOARD};

    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *timer_fps;

  public:
    AllegroEngine();
    ~AllegroEngine();
    ALLEGRO_DISPLAY *root;
    ALLEGRO_FONT *font;

    bool Init(int winwidth, int winheight);
    void Start();
    void End();

    virtual bool EngineInit(map<int,bool> errormap) {return true;}
    virtual void Update(){}
    virtual void Render(ALLEGRO_DISPLAY *root){}
    virtual void EngineEnd(){};
    void HandleTimers(ALLEGRO_TIMER *timer, int count){if (timer = timer_fps) {Update(); DoRender();} else TimerTick(timer, count);}
    //
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    //
    virtual void KeyUp(const int &key,const int &unicode, const int &mods){}
    virtual void KeyDown(const int &key,const int &unicode, const int &mods, const bool &repeated = 0){}
    virtual void MouseMoved(/*const int &iButton,*/const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ){}
    virtual void MouseButtonUp(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ){}
    virtual void MouseButtonDown(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ){}
    virtual void TimerTick(ALLEGRO_TIMER *timer, int count){}
  };

//extern ALLEGRO_COLOR GetColor(int r,int g,int b);
//extern inline int clamp(int x,int lo,int hi);