
#pragma once

#include <SDL2/SDL.h>


typedef void (*fnHandleEvent)(SDL_Event &event, float x_scale, float y_scale);
typedef void (*fnRenderBackbuffer)();


class Window {
private:
    SDL_Window* win;
    SDL_Renderer* rend;
    int width;
    int height;
    float x_scale;
    float y_scale;

public:
    Window(const char *title, int _width, int _height);
    ~Window();
    void RunLoop(fnHandleEvent HandleEvent, fnRenderBackbuffer RenderBackbuffer);
    void Paint(unsigned char *BackBuffer);
};


extern Window *gameWindow;

