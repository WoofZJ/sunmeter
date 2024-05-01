#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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

  SDL_Window *window = SDL_CreateWindow("Window", 352, 430, SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
  if (!window) {
    return SDL_Fail();
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, 0);
  if (!renderer) {
    return SDL_Fail();
  }

  // print some information about the window
  SDL_ShowWindow(window);
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
        SDL_SetWindowPosition(app->window, win_x+x-start_x, win_y+y-start_y);
      }
      break;

  }

  return 0;
}

int SDL_AppIterate(void *appstate) {
    auto* app = (AppContext*)appstate;

    // draw a color
    auto time = SDL_GetTicks() / 1000.f;
    auto red = (std::sin(time) + 1) / 2.0 * 255;
    auto green = (std::sin(time / 2) + 1) / 2.0 * 255;
    auto blue = (std::sin(time) * 2 + 1) / 2.0 * 255;
    
    SDL_SetRenderDrawColor(app->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);
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
