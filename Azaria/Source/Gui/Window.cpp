
#include "Window.h"
#include "display.h"
#include "SDL2/SDL.h"

Window *gameWindow;


Window::Window(const char *title, int _width, int _height) {
    x_scale = 2;
    y_scale = 2;
    width = _width;
    height = _height;

	win = SDL_CreateWindow(title,
							SDL_WINDOWPOS_CENTERED, 
							SDL_WINDOWPOS_CENTERED, 
							width*x_scale,
                            height*y_scale,
                            SDL_WINDOW_RESIZABLE); 

	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); 
}

Window::~Window() {
	SDL_DestroyRenderer(rend); 
	SDL_DestroyWindow(win); 
}

void Window::RunLoop(fnHandleEvent HandleEvent, fnRenderBackbuffer RenderBackbuffer) {
    	int close = 0; 
	while (!close) { 
		SDL_Event event; 

		// Events mangement 
		while (SDL_PollEvent(&event)) { 
			if (event.type == SDL_QUIT) {
				// handling of close button 
				close = 1; 
				break; 
            } else {
                switch (event.type) {
                    case SDL_WINDOWEVENT:
                        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                            x_scale = (float)(event.window.data1) / width;
                            y_scale = (float)(event.window.data2) / height;
                            printf("resized\n");
                        }
                }
                HandleEvent(event, x_scale, y_scale);
            }
		}

		// clears the screen 
		SDL_RenderClear(rend);
        RenderBackbuffer();

		// triggers the double buffers 
		// for multiple rendering 
		SDL_RenderPresent(rend); 

		// calculates to 60 fps 
		SDL_Delay(1000 / 120); 
	} 
}

void Window::Paint(unsigned char *data) {
    // Set up the pixel format color masks for RGB(A) byte arrays.
    // Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (req_format == STBI_rgb) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
    #else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0x00000000;
    #endif

    int depth=32;
    int pitch = 4*width;

    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)data, width, height, depth, pitch,
                                             rmask, gmask, bmask, amask);

    if (surf == NULL) {
        SDL_Log("Creating surface failed: %s", SDL_GetError());
    }

	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf); 

    // ... do something useful with the surface ...
    // ...
	SDL_RenderCopy(rend, tex, NULL, NULL); 

    // when you don't need the surface anymore, free it..
    SDL_FreeSurface(surf);
   	SDL_DestroyTexture(tex); 

   	display.SetAllDirty();
}

