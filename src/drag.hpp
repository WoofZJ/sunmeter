#ifndef __SUNMETER_DRAG_HPP__
#define __SUNMETER_DRAG_HPP__

#include <SDL3/SDL.h>

bool SMT_IsDraggable();
void SMT_SetDraggable(bool draggble);

void __SMT_Drag_ProcessEvent(SDL_Window *window, const SDL_Event *event);

#endif