#include "drag.hpp"

static bool smt_draggable, drag, resize;
static int win_x, win_y, pos_x, pos_y, dir, w, h, gap, size_x, size_y, cur, temp;
static float start_x, start_y, x, y;
static SDL_Cursor* cursors[5];

bool SMT_IsDraggable() {
  return smt_draggable;
}
void SMT_SetDraggable(bool draggble) {
  smt_draggable = draggble;
  if (draggble) {
    gap = 20;
    cursors[0] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    cursors[1] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    cursors[2] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    cursors[3] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    cursors[4] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
  } else {
    for (int i = 0; i < 5; ++i) {
      SDL_DestroyCursor(cursors[i]);
    }
  }
}

void __SMT_Drag_ProcessEvent(SDL_Window *window, const SDL_Event *event) {
  if (!smt_draggable) {
    return;
  }
  switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (event->button.button == SDL_BUTTON_LEFT) {
        SDL_GetWindowPosition(window, &win_x, &win_y);
        pos_x = win_x;
        pos_y = win_y;
        SDL_GetGlobalMouseState(&start_x, &start_y);
        SDL_GetWindowSize(window, &w, &h);
        if (event->motion.x < gap) {
          dir |= 1;
        } else if (event->motion.x > w-gap) {
          dir |= 2;
        }
        if (event->motion.y < gap) {
          dir |= 4;
        } else if (event->motion.y > h-gap) {
          dir |= 8;
        }
        if (dir == 0) {
          drag = true;
        } else {
          resize = true;
          SDL_GetWindowSize(window, &size_x, &size_y);
        }
      }
      break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      if (event->button.button == SDL_BUTTON_LEFT) {
        drag = false;
        resize = false;
        dir = 0;
      }
      break;
    
    case SDL_EVENT_MOUSE_MOTION:
      temp = 0;
      if (event->motion.x < gap) {
        temp |= 1;
      } else if (event->motion.x > w-gap) {
        temp |= 2;
      }
      if (event->motion.y < gap) {
        temp |= 4;
      } else if (event->motion.y > h-gap) {
        temp |= 8;
      }
      if ((temp == 0b1 || temp == 0b10) && cur != 1) {
        cur = 1;
        SDL_SetCursor(cursors[1]);
      }
      if ( (temp == 0b100 || temp == 0b1000) && cur != 2) {
        cur = 2;
        SDL_SetCursor(cursors[2]);
      }
      if ( (temp == 0b0110 || temp == 0b1001) && cur != 3) {
        cur = 3;
        SDL_SetCursor(cursors[3]);
      }
      if ( (temp == 0b0101 || temp == 0b1010) && cur != 4) {
        cur = 4;
        int status = SDL_SetCursor(cursors[4]);
      }
      if (temp == 0 && cur != 0) {
        cur = 0;
        int status = SDL_SetCursor(cursors[0]);
      }

      if (drag) {
        SDL_GetGlobalMouseState(&x, &y);
        SDL_SetWindowPosition(window, win_x+x-start_x, win_y+y-start_y);
      }
      else if (resize) {
        SDL_GetGlobalMouseState(&x, &y);
        if (dir & 1) {
          size_x = w-(x-start_x);
          pos_x = win_x+(x-start_x);
        } else if (dir & 2) {
          size_x = w+x-start_x;
        }
        if (dir & 4) {
          size_y = h - (y-start_y);
          pos_y = win_y+(y-start_y);
        } else if (dir & 8) {
          size_y = h+(y-start_y);
        }
        SDL_SetWindowSize(window, size_x, size_y);
        SDL_SetWindowPosition(window, pos_x, pos_y);
      }
      break;
  }
}