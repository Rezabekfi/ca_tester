#pragma once

#include <cstddef>
#include "core/engine.hpp"
#include <SDL.h>
#include <imgui.h>

// WARNING: This class is a bit of a mess and could use some refactoring.
// The issue lies in the fact that UI has to set calculating distance for convex hull which is obviously not a UI concern
// I'll change it as soon as possible but now it stays

// Handles rendering + UI 
class Renderer {
public:
  // Takes engine by reference, user can drive the simulation via the UI
  Renderer(Engine& engine, std::size_t cell_size = 20);
  ~Renderer();

  // Main frame function (called in loop); returns false when app should exit
  bool render();

private:
  Engine& engine_; // central dependency

  std::size_t cell_size_; // base size of each cell (zoom modifies it)
  float zoom_;

  float left_width_; 
  float speed_from_slider_; // UI value → synced with engine speed
  float min_left_;
  float splitter_w_;

  bool paused_; // UI-side pause state 
  bool light_mode_;          
  bool show_grid_;
  bool autosize_os_window_; // this is set to false as it was causing some issues 

  std::size_t iteration_; // cached from engine 
  std::size_t iterations_per_step_; // allows stepping multiple times per frame

  std::size_t grid_rows_;
  std::size_t grid_cols_;

  Neighborhood neighborhood_; // UI copy (syncs to engine when changed)
  Boundary boundary_;

  std::size_t hovered_cell_x_; // for interaction/debug
  std::size_t hovered_cell_y_;

  // Raw SDL handles (manual lifetime management)
  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;

  // UI panels (split into smaller pieces for sanity)
  void renderControls();
  void renderGrid();
  void renderSplitter();
  void renderGridSettings();
  void renderNeighborhoodSettings();
  void renderBoundarySettings();
  void renderRuleSettings();
  void renderCustomRuleEditor(); 
  void renderAbout();

  void renderExportImportButtons();

  bool drawGrid(float line_thickness = 1.0f,
                ImU32 gridColor = IM_COL32(50, 50, 50, 255),
                ImU32 fillColor = IM_COL32(200, 200, 200, 255));

  // ImGui lifecycle
  void setUpImGui();
  void tearDownImGui();

  // Applies dark/light theme depending on UI state
  void applyImGuiTheme();

  // Initializes SDL (window + renderer)
  bool sdl_init();
};
