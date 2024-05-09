#include "label.hpp"

Label::Label(string text, int font_size, SDL_FRect rect, SDL_Color color) {
  this->font_size = font_size;
  this->rect = rect;
  this->color = color;
  this->texture = NULL;
  this->surface = NULL;

  font = TTF_OpenFont("Youyuan.ttf", font_size);
  this->setText(text);
}

void Label::render(SDL_Renderer *renderer) {
  if (texture == NULL && surface != NULL) {
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
    }
  }
  if (!text.empty()) {
    SDL_RenderTexture(renderer, texture, NULL, &rect);
  }
}

void Label::setText(string text) {
  if (!text.empty()) {
    this->text = text;
    updateContent();
  }
}

void Label::setPosition(float x, float y) {
  this->rect.x = x;
  this->rect.y = y;
}

void Label::setFontSize(int size) {
  if (font_size != size) {
    TTF_SetFontSize(font, size);
    font_size = size;
    updateContent();
  }
}

void Label::updateContent() {
  if (surface != NULL) {
    SDL_DestroySurface(surface);
  }
  if (texture != NULL) {
    SDL_DestroyTexture(texture);
  }
  texture = NULL;
  surface = TTF_RenderUTF8_Blended_Wrapped(font, this->text.c_str(), color, 0);
  if (surface == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, TTF_GetError());
  } else {
    this->rect.w = surface->w;
    this->rect.h = surface->h;
  }
}