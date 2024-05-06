#ifndef __LABEL_HPP__
#define __LABEL_HPP__

#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

using std::string;

class Label {
public:
  Label(string text, int font_size, SDL_FRect rect, SDL_Color color);
  void setPosition(float x, float y);
  void setText(string text);
  void render(SDL_Renderer *renderer);

private:
  SDL_FRect rect;
  SDL_Color color;
  int font_size;
  string text;
  SDL_Surface *surface;
  SDL_Texture *texture;
  TTF_Font *font;
  
};


#endif