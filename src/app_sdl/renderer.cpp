#include "renderer.hpp"
#include <SDL_video.h>
#include <imgui.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <stdexcept>
#include "core/rule_registry.hpp"
#include "convex_hull/convex_hull.hpp"
#include <filesystem>
#include "core/io.hpp"
#include <iostream>

// this class is a bit of a mess sorry 
// it is riddled with magic numbers

// Sets up SDL + initial UI state
Renderer::Renderer(Engine& engine, std::size_t cell_size)
  : engine_(engine), cell_size_(cell_size) {
  if (!sdl_init()) {
    throw std::runtime_error("Failed to initialize SDL");
  }

  zoom_ = 1.0f;
  left_width_ = 300.0f;
  min_left_ = 220.0f;
  splitter_w_ = 6.0f;
  paused_ = true;
  speed_from_slider_ = 1.0f;
  autosize_os_window_ = false;
  iteration_ = 0;
  iterations_per_step_ = 1;
  grid_rows_ = engine_.getGrid().getHeight();
  grid_cols_ = engine_.getGrid().getWidth();
  neighborhood_ = Neighborhood::Moore;
  boundary_ = Boundary::Wrap;
  light_mode_ = false;
  show_grid_ = true;

  setUpImGui();
}

// Runs one UI frame returns false when window should close
// this is in the main loop
bool Renderer::render() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    ImGui_ImplSDL2_ProcessEvent(&e);

    if (e.type == SDL_QUIT) {
      return false;
    }

    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
      return false;
    }
  }

  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // Root window sizes itself around controls + grid
  ImGuiWindowFlags rootFlags = ImGuiWindowFlags_AlwaysAutoResize
                              | ImGuiWindowFlags_NoSavedSettings
                              | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Cellular Automata Simulator", nullptr, rootFlags);

  renderControls();
  renderSplitter();
  renderGrid();

  ImGui::End();
  ImGui::Render();

  // Match SDL clear color to current UI theme
  if (light_mode_) {
    SDL_SetRenderDrawColor(sdl_renderer_, 240, 240, 240, 255);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer_, 18, 18, 18, 255);
  }

  SDL_RenderClear(sdl_renderer_);
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdl_renderer_);
  SDL_RenderPresent(sdl_renderer_);

  // Push UI values back into engine after frame render
  engine_.setSpeed(speed_from_slider_);
  iteration_ = engine_.getIteration();

  return true;
}

// Draggable divider between controls and grid
void Renderer::renderSplitter() {
  ImGui::SameLine();
  ImGui::InvisibleButton("Splitter", ImVec2(splitter_w_, -1));

  if (ImGui::IsItemActive()) {
    float mouse_x = ImGui::GetIO().MousePos.x;
    left_width_ = std::max(min_left_, left_width_ + mouse_x);
  }

  // Visual handle for the invisible splitter
  { // this scope shouldnt be needed used from example code
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 sp = ImGui::GetItemRectMin();
    ImVec2 se = ImGui::GetItemRectMax();

    dl->AddRectFilled(sp, se, IM_COL32(150,150,150,120));
    dl->AddLine(ImVec2(sp.x + splitter_w_*0.5f, sp.y),
                ImVec2(sp.x + splitter_w_*0.5f, se.y),
                IM_COL32(90,90,90,255), 1.0f); 
  }
}

// Left-side control panel
void Renderer::renderControls() {
  ImGui::BeginChild("Controls", ImVec2(left_width_, 0), true);
  {
    ImGui::Text("Simulation Controls");
    ImGui::Separator();

    if (ImGui::Checkbox("Light mode", &light_mode_)) {
      applyImGuiTheme(); // theme needs reapplying immediately
    }

    ImGui::Checkbox("Show grid", &show_grid_);
    ImGui::Separator();

    ImGui::Text("Iteration: %zu", iteration_);

    ImGui::Text("Iterations per Step");
    ImGui::InputScalar("##step_iters", ImGuiDataType_U32, &iterations_per_step_);

    ImGui::SliderFloat("Speed", &speed_from_slider_, 0.0f, 100.0f, "%.1f");

    if (ImGui::Button(paused_ ? "Start" : "Pause")) {
      engine_.toggleRunning();
      paused_ = !paused_;
    }

    ImGui::SameLine();

    if (paused_ && ImGui::Button("Step")) {
      // Manual stepping can batch multiple generations
      for (std::size_t i = 0; i < iterations_per_step_; ++i) {
        engine_.step();
      }

    } else if (!paused_) {
      ImGui::BeginDisabled();
      ImGui::Button("Step");
      ImGui::EndDisabled();
    }

    ImGui::SameLine();

    if (paused_ && ImGui::Button("Back")) {
      engine_.stepBack(iterations_per_step_);
    } else if (!paused_) {
      ImGui::BeginDisabled();
      ImGui::Button("Back");
      ImGui::EndDisabled();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
      paused_ = true;
      engine_.reset();
    }

    ImGui::SameLine();

    if (paused_ && ImGui::Button("Clear")) {
      engine_.clean();
    } else if (!paused_) {
      ImGui::BeginDisabled();
      ImGui::Button("Clear");
      ImGui::EndDisabled();
    }

    // History jump is only safe while paused
    static uint64_t wanted_iteration = 0;
    ImGui::Text("Go to Iteration:");
    ImGui::InputScalar("##goto_iter", ImGuiDataType_U64, &wanted_iteration);

    if (paused_ && ImGui::Button("Go")) {
      engine_.goToIteration(wanted_iteration);
    } else if (!paused_) {
      ImGui::BeginDisabled();
      ImGui::Button("Go");
      ImGui::EndDisabled();
    }

    renderNeighborhoodSettings();
    renderBoundarySettings();
    renderRuleSettings();

    ImGui::Separator();

    renderGridSettings();
    renderExportImportButtons();
  }
  ImGui::EndChild();
}

// Neighborhood dropdown locked while simulation is running
void Renderer::renderNeighborhoodSettings() {
  const char* current_neighborhood = neighborhoodToString(neighborhood_);
  const bool disable = !paused_;

  if (disable) ImGui::BeginDisabled();

  const bool open = ImGui::BeginCombo("Neighborhood", current_neighborhood);
  if (open) {
    for (int n = 0; n < static_cast<int>(Neighborhood::Count); ++n) {
      bool is_selected = (neighborhood_ == static_cast<Neighborhood>(n));

      if (ImGui::Selectable(neighborhoodToString(static_cast<Neighborhood>(n)), is_selected)) {
        neighborhood_ = static_cast<Neighborhood>(n);
        engine_.setNeighborhood(neighborhood_);
      }

      if (is_selected) ImGui::SetItemDefaultFocus();
    }

    ImGui::EndCombo();
  }

  if (disable) ImGui::EndDisabled();
}

// Rule dropdown + rule-specific setup
// This is an issue 
void Renderer::renderRuleSettings() {
  auto list = RuleRegistry::getInstance().list();
  std::string current_rule_name = engine_.getRule().getName();
  const char* current_rule = current_rule_name.c_str();
  const bool disable = !paused_;

  if (disable) ImGui::BeginDisabled();

  const bool open = ImGui::BeginCombo("Rule", current_rule);
  if (open) {
    for (const auto& rule_entry : list) {
      bool is_selected = (engine_.getRule().getName() == rule_entry.key);

      if (ImGui::Selectable(rule_entry.key.c_str(), is_selected)) {
        engine_.setRule(RuleRegistry::getInstance().make(rule_entry.key));

        if (rule_entry.key == CONVEX_HULL_RULE_NAME) {
          // Convex hull needs distance bits prepared before stepping
          engine_.setCalculatingDistances(true);
          engine_.setDistanceCalculator(ConvexHull::calculateDistances);

          // Existing seeds become origins for distance propagation
          for (auto& cell : engine_.getGrid().getGridValues()) {
            if (is_seed(cell)) {
              cell = mark_origin(cell); 
            }
          }
        } else {
          // Other rules should not inherit convex-hull helper bits
          engine_.setCalculatingDistances(false);
          engine_.resetDistances();
        }
      }

      if (is_selected) ImGui::SetItemDefaultFocus();
    }

    ImGui::EndCombo();
  }

  if (disable) ImGui::EndDisabled();
}

// Boundary dropdown locked while running
void Renderer::renderBoundarySettings() {
  const char* current_boundary = boundaryToString(boundary_);
  const bool disable = !paused_;

  if (disable) ImGui::BeginDisabled();

  const bool open = ImGui::BeginCombo("Boundary", current_boundary);
  if (open) {
    for (int b = 0; b < static_cast<int>(Boundary::Count); ++b) {
      bool is_selected = (boundary_ == static_cast<Boundary>(b));

      if (ImGui::Selectable(boundaryToString(static_cast<Boundary>(b)), is_selected)) {
        boundary_ = static_cast<Boundary>(b);
        engine_.setBoundary(boundary_);
      }

      if (is_selected) ImGui::SetItemDefaultFocus();
    }

    ImGui::EndCombo();
  }

  if (disable) ImGui::EndDisabled();
}

// Grid display/edit settings
void Renderer::renderGridSettings() {
  ImGui::Text("Grid Settings");
  ImGui::SliderFloat("Zoom", &zoom_, 0.1f, 5.0f, "%.1f");
  ImGui::Separator();

  ImGui::Text("Grid Dimensions");

  static int pending_rows = grid_rows_;
  static int pending_cols = grid_cols_;

  bool changed_dims = false;
  changed_dims |= ImGui::InputInt("Rows", &pending_rows);
  changed_dims |= ImGui::InputInt("Cols", &pending_cols);

  // Avoid invalid grid sizes from user input
  pending_rows = std::max(1, pending_rows);
  pending_cols = std::max(1, pending_cols);

  if (paused_ && ImGui::Button("Apply Size")) {
    grid_rows_ = pending_rows; 
    grid_cols_ = pending_cols;
    engine_.resizeGrid(grid_cols_, grid_rows_);
  } else if (!paused_) {
    ImGui::BeginDisabled();
    ImGui::Button("Apply Size");
    ImGui::EndDisabled();
  }

  ImGui::Text("Mouse over cell: (%zu, %zu)", hovered_cell_x_, hovered_cell_y_);
}

// Draws grid and handles paused editing
void Renderer::renderGrid() {
  Grid& grid = engine_.getGrid();

  std::size_t rows = grid.getGridValues().size() / grid.getWidth();
  std::size_t cols = grid.getWidth();

  // Local copy avoids drawing while grid mutates underneath
  std::vector<uint8_t> cells = grid.getGridValues();

  const float cellSize = cell_size_ * zoom_;
  const float grid_w = cols * cellSize;
  const float grid_h = rows * cellSize;
  const float line_thickness = 1.0f;

  // Theme-aware colors
  const ImU32 gridColor   = light_mode_
                            ? IM_COL32(200, 200, 200, 255)
                            : IM_COL32(50, 50, 50, 255);
  const ImU32 fillColor   = light_mode_
                            ? IM_COL32(40, 40, 40, 255)
                            : IM_COL32(200, 200, 200, 255);
  const ImU32 bgColor     = light_mode_
                            ? IM_COL32(250, 250, 250, 255)
                            : IM_COL32(25, 25, 25, 255);
  const ImU32 borderColor = light_mode_
                            ? IM_COL32(100, 100, 100, 255)
                            : IM_COL32(130, 130, 130, 255);

  ImGui::SameLine();

  // Grid child has no padding so cells align exactly with the canvas
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::BeginChild(
      "GridPane",
      ImVec2(grid_w, grid_h),
      false,
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
  );
  {
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    const ImVec2 size(grid_w, grid_h);

    // Invisible hitbox used for mouse editing
    ImGui::InvisibleButton("##grid", size);

    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRectFilled(p0, ImVec2(p0.x + size.x, p0.y + size.y), bgColor);
    dl->AddRect(p0, ImVec2(p0.x + size.x, p0.y + size.y), borderColor, 0.0f, 0, line_thickness);

    // Draw active/marked cells
    for (std::size_t r = 0; r < rows; ++r) {
      for (std::size_t c = 0; c < cols; ++c) {
        if (cells[r * cols + c] & 0x03) { // low bits: seed/marked state
          ImVec2 a(p0.x + c * cellSize + 1, p0.y + r * cellSize + 1);
          ImVec2 b(p0.x + (c + 1) * cellSize - 1, p0.y + (r + 1) * cellSize - 1);
          dl->AddRectFilled(a, b, fillColor);
        }
      }
    }
    
    if (show_grid_) {
        for (std::size_t c = 1; c < cols; ++c) {
            float x = p0.x + c * cellSize;
            dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p0.y + size.y),
                        gridColor, line_thickness);
        }

        for (std::size_t r = 1; r < rows; ++r) {
            float y = p0.y + r * cellSize;
            dl->AddLine(ImVec2(p0.x, y), ImVec2(p0.x + size.x, y),
                        gridColor, line_thickness);
        }
    }

    // Editing is disabled while simulation thread may be writing
    if ((hovered || active) && paused_) {
      ImVec2 mp = ImGui::GetIO().MousePos;
      int col = (int)((mp.x - p0.x) / cellSize);
      int row = (int)((mp.y - p0.y) / cellSize);

      hovered_cell_x_ = col;
      hovered_cell_y_ = row;

      if (col >= 0 && col < (int)cols && row >= 0 && row < (int)rows) {
        int idx = row * cols + col;

        // Click toggles current cell
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
          cells[idx] ^= 1u;
          grid.setCell(col, row, cells[idx]);
        }

        // Drag paints, Shift+drag erases
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
          bool erase = ImGui::GetIO().KeyShift;
          uint8_t newVal = erase ? 0u : 1u;

          if (cells[idx] != newVal) {
            cells[idx] = newVal;
            grid.setCell(col, row, cells[idx]);
          }
        }

        // Lightweight hover feedback
        ImVec2 a(p0.x + col * cellSize + 1,       p0.y + row * cellSize + 1);
        ImVec2 b(p0.x + (col + 1) * cellSize - 1, p0.y + (row + 1) * cellSize - 1);
        dl->AddRect(a, b, IM_COL32(255, 255, 255, 80));
      }
    }
  }

  ImGui::EndChild();
  ImGui::PopStyleVar();
}

// Cleans up UI + SDL resources
Renderer::~Renderer() {
  tearDownImGui();
}

// Initializes ImGui bindings for SDL renderer backend
void Renderer::setUpImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  applyImGuiTheme();

  ImGui_ImplSDL2_InitForSDLRenderer(window_, sdl_renderer_);
  ImGui_ImplSDLRenderer2_Init(sdl_renderer_);
}

// Applies current theme to ImGui
void Renderer::applyImGuiTheme() {
  if (light_mode_) {
    ImGui::StyleColorsLight();
  } else {
    ImGui::StyleColorsDark();
  }
}

// Releases ImGui and SDL resources
void Renderer::tearDownImGui() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  if (sdl_renderer_) {
    SDL_DestroyRenderer(sdl_renderer_);
    sdl_renderer_ = nullptr;
  }

  if (window_) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }

  SDL_Quit();
}

// Creates SDL window + hardware renderer
bool Renderer::sdl_init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    return false;
  }

  window_ = SDL_CreateWindow(
      "Cellular Automata Simulator",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      1280, 720,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN 
  );

  if (!window_) {
    return false;
  }

  sdl_renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!sdl_renderer_) {
    SDL_DestroyWindow(window_);
    return false;
  }

  return true;
}

// Placeholder for project/app info panel
void Renderer::renderAbout() {
  // I didnt do it in time
  // TODO: 
}

// Save/load controls for grid snapshots
void Renderer::renderExportImportButtons() {
  ImGui::Text("Save/Load Grid:");

  if (paused_ && ImGui::Button("Save Grid")) {
    ImGui::OpenPopup("Save Grid");
  }

  if (ImGui::BeginPopup("Save Grid")) {
    static char save_filename[128] = "grid_save.json";
    static bool use_default_save_folder = true;

    ImGui::InputText("Filename", save_filename, IM_ARRAYSIZE(save_filename));
    ImGui::Checkbox("Use Default Save Folder", &use_default_save_folder);

    if (ImGui::Button("Save")) {
      bool success = IO::instance().saveGridToFile(engine_, std::string(save_filename), use_default_save_folder);
      ImGui::CloseCurrentPopup();

      if (!success) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Save Error",
                                 "Failed to save the grid to file.",
                                 window_);
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  if (paused_ && ImGui::Button("Load Grid")) {
    ImGui::OpenPopup("Load Grid");
  }

  if (ImGui::BeginPopup("Load Grid")) {
    static char load_filename[128] = "grid_save.json";

    ImGui::InputText("Filename", load_filename, IM_ARRAYSIZE(load_filename));

    if (ImGui::Button("Load")) {
      bool success = IO::instance().loadGridFromFile(engine_, std::string(load_filename));
      ImGui::CloseCurrentPopup();

      if (!success) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Load Error",
                                 "Failed to load the grid from file.",
                                 window_);
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}
