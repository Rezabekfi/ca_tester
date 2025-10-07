#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "core/engine.hpp"
#include <SDL.h>


class Renderer {
public:
  Renderer(Engine& engine, std::size_t cell_size = 20);
  ~Renderer();
  void render();
private:
  Engine& engine_;
  std::size_t cell_size_;
  float zoom_;
  float left_width_; 
  float speed_from_slider_;
  float min_left_;
  float splitter_w_;
  bool paused_;
  bool autosize_os_window_;
  std::size_t iteration_;

  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;



  void renderControls();
  void renderGrid();
  void renderSplitter();

  // More private methods will be added as I figure out what is needed

  void setUpImGui();
  void tearDownImGui();

  bool sdl_init(); // TODO: consider renaming this


};
