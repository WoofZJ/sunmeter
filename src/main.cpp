#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>

struct AppContext {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_bool app_quit = SDL_FALSE;
};

int SDL_Fail() {
  SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
  return -1;
}

int SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_Fail();
  }

  if (TTF_Init()) {
    SDL_Log(TTF_GetError());
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Window", 352, 430, SDL_WINDOW_BORDERLESS);
  if (!window) {
    return SDL_Fail();
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, 0);
  if (!renderer) {
    return SDL_Fail();
  }

  // print some information about the window
  SDL_ShowWindow(window);
  SDL_SetWindowOpacity(window, 0.9);
  {
    int width, height, bbwidth, bbheight;
    SDL_GetWindowSize(window, &width, &height);
    SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
    SDL_Log("Window size: %ix%i", width, height);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
    if (width != bbwidth) {
      SDL_Log("This is a highdpi environment.");
    }
  }

  // set up the application data
  *appstate = new AppContext{
      window,
      renderer,
  };

  SDL_Log("Application started successfully!");

  return 0;
}

int SDL_AppEvent(void *appstate, const SDL_Event *event) {
  static bool drag = false;
  static float start_x, start_y;
  static int win_x, win_y;
  auto *app = (AppContext *)appstate;

  if (event->type == SDL_EVENT_QUIT) {
    app->app_quit = SDL_TRUE;
  }
  switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (event->button.button == SDL_BUTTON_LEFT) {
        SDL_Log("Left button down!");
        drag = true;
        SDL_GetWindowPosition(app->window, &win_x, &win_y);
        SDL_GetGlobalMouseState(&start_x, &start_y);
        {
          float x, y;
          SDL_GetGlobalMouseState(&x, &y);
          SDL_Log("Mouse global pos: (%.1f, %.1f)", x, y);
          SDL_Rect rect;
          SDL_GetDisplayBounds(1, &rect);
          SDL_Log("Display size: (%dx%d)", rect.w, rect.h);
        }
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      if (event->button.button == SDL_BUTTON_LEFT) {
        SDL_Log("Left button up!");
        drag = false;
      }
      break;
    
    case SDL_EVENT_MOUSE_MOTION:
      if (drag) {
        float x, y;
        SDL_GetGlobalMouseState(&x, &y);
        SDL_Rect rect;
        SDL_GetDisplayBounds(1, &rect);
        int dest_x = win_x+x-start_x, dest_y = win_y+y-start_y;
        dest_x = (dest_x / (rect.w/15)) * (rect.w/15);
        dest_y = (dest_y / (rect.h/10)) * (rect.h/10);
        SDL_SetWindowPosition(app->window, dest_x, dest_y);
      }
      break;
    
    case SDL_EVENT_WINDOW_MINIMIZED:
      SDL_Log("Window minimized!");
      SDL_Log("state: %d", SDL_GetWindowFlags(app->window) & SDL_WINDOW_MINIMIZED);
      SDL_RestoreWindow(app->window);
      break;

    case SDL_EVENT_WINDOW_RESTORED:
      SDL_Log("Window restored!");
      SDL_Log("state: %d", SDL_GetWindowFlags(app->window) & SDL_WINDOW_MINIMIZED);
      break;
  }

  return 0;
}

int SDL_AppIterate(void *appstate) {
    auto* app = (AppContext*)appstate;
    static TTF_Font *font;
    static SDL_Color color {128, 255, 128, 255};
    static SDL_Surface* surface = NULL;
    static SDL_Texture* texture;
    static SDL_FRect rect {0, 0, 120, 24};
    if (surface == NULL) {
      font = TTF_OpenFont("youyuan.TTF", 24);
      if (!font) {
        SDL_Log(TTF_GetError());
        return -1;
      }
      surface = TTF_RenderUTF8_Blended(font, "你好", color);
      if (!surface) {
        SDL_Log(TTF_GetError());
        return -1;
      }
      rect.w = surface->w;
      rect.h = surface->h;
      texture = SDL_CreateTextureFromSurface(app->renderer, surface);
      if (!texture) {
        return SDL_Fail();
      }
    }
    // draw a color
    rect.x += 1;
    if (rect.x > 300) {
      rect.x = 0;
    }
    SDL_RenderClear(app->renderer);
    SDL_RenderTexture(app->renderer, texture, NULL, &rect);
    SDL_RenderPresent(app->renderer);

    return app->app_quit;
}

void SDL_AppQuit(void* appstate) {
    auto* app = (AppContext*)appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    SDL_Quit();
    SDL_Log("Application quit successfully!");
}
