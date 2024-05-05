#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include "drag.hpp"

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

  SDL_Window *window = SDL_CreateWindow("Window", 800, 400, SDL_WINDOW_BORDERLESS | SDL_WINDOW_TRANSPARENT);
  if (!window) {
    return SDL_Fail();
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, 0);
  if (!renderer) {
    return SDL_Fail();
  }

  SMT_SetDraggable(true);
  // print some information about the window
  SDL_ShowWindow(window);
  // SDL_SetWindowOpacity(window, 0.9);
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
  auto *app = (AppContext *)appstate;

  if (event->type == SDL_EVENT_QUIT) {
    app->app_quit = SDL_TRUE;
  }

  __SMT_Drag_ProcessEvent(app->window, event);
  switch (event->type) {
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
    static SDL_Color color {255, 0, 0, 255};
    static SDL_FRect rect {0, 0, 120, 24};
    static auto start = SDL_GetTicks();
    int w, h;
    int gap = 20;
    SDL_GetWindowSize(app->window, &w, &h);

    auto time = SDL_GetTicks();

    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderClear(app->renderer);
    SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255);
    SDL_RenderLine(app->renderer, 0, gap, w, gap);
    SDL_RenderLine(app->renderer, 0, h-gap, w, h-gap);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 255, 255);
    SDL_RenderLine(app->renderer, gap, 0, gap, h);
    SDL_RenderLine(app->renderer, w-gap, 0, w-gap, h);
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
