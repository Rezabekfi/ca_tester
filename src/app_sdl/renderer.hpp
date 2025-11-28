#pragma once

#include <cstddef>
#include "core/engine.hpp"
#include <SDL.h>
#include <imgui.h>

class Renderer {
public:
  Renderer(Engine& engine, std::size_t cell_size = 20);
  ~Renderer();
  bool render();
private:
  Engine& engine_;
  std::size_t cell_size_;
  float zoom_;
  float left_width_; 
  float speed_from_slider_;
  float min_left_;
  float splitter_w_;
  bool paused_;
  bool light_mode_;          // <--- used for light/dark theme
  bool autosize_os_window_;
  std::size_t iteration_;
  std::size_t iterations_per_step_;
  std::size_t grid_rows_;
  std::size_t grid_cols_;
  Neighborhood neighborhood_;
  Boundary boundary_;

  // left as pointers because of SDL
  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;

  void renderControls();
  void renderGrid();
  void renderSplitter();
  void renderGridSettings();
  void renderNeighborhoodSettings();
  void renderBoundarySettings();
  void renderRuleSettings();
  void renderCustomRuleEditor(); 
  void renderAbout();

  bool drawGrid(float line_thickness = 1.0f,
                ImU32 gridColor = IM_COL32(50, 50, 50, 255),
                ImU32 fillColor = IM_COL32(200, 200, 200, 255));

  void setUpImGui();
  void tearDownImGui();
  void applyImGuiTheme();    // <--- new helper

  bool sdl_init(); // TODO: consider renaming this
};

