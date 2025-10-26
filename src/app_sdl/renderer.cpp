#include "renderer.hpp"
#include <SDL_video.h>
#include <imgui.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <stdexcept>


Renderer::Renderer(Engine& engine, std::size_t cell_size)
  : engine_(engine), cell_size_(cell_size) {
  if (!sdl_init()) {
    throw std::runtime_error("Failed to initialize SDL");
  }
  setUpImGui();
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
}

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

  // Root window AUTO-SIZED to its content
  ImGuiWindowFlags rootFlags = ImGuiWindowFlags_AlwaysAutoResize
                              | ImGuiWindowFlags_NoSavedSettings
                              | ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Cellular Automata Simulator", nullptr, rootFlags);

  renderControls();
  renderSplitter();
  renderGrid();
  ImGui::End();

  ImGui::Render();
  SDL_SetRenderDrawColor(sdl_renderer_, 18, 18, 18, 255);
  SDL_RenderClear(sdl_renderer_);
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdl_renderer_);
  SDL_RenderPresent(sdl_renderer_);

  // update stats
  engine_.setSpeed(speed_from_slider_);
  iteration_ = engine_.getIteration();

  return true;
}

void Renderer::renderSplitter() {
  ImGui::SameLine();
  ImGui::InvisibleButton("Splitter", ImVec2(splitter_w_, -1));
  if (ImGui::IsItemActive()) {
    float mouse_x = ImGui::GetIO().MousePos.x;
    left_width_ = std::max(min_left_, left_width_ + mouse_x);
  }
  // Draw splitter line
  { // this scope shouldnt be needed used from example code
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 sp = ImGui::GetItemRectMin();
    ImVec2 se = ImGui::GetItemRectMax();
    dl->AddRectFilled(sp, se, IM_COL32(150,150,150,120));
    dl->AddLine(ImVec2(sp.x + splitter_w_*0.5f, sp.y),
                ImVec2(sp.x + splitter_w_*0.5f, se.y),
                IM_COL32(90,90,90,255), 1.0f); // Check this line later
  }
}

void Renderer::renderControls() {
  ImGui::BeginChild("Controls", ImVec2(left_width_, 0), true);
  {
    ImGui::Text("Simulation Controls");
    ImGui::Separator();
    // display iteration
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
    // let user input iteration to jump to (only backwards)
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
    ImGui::Separator();
    renderGridSettings();
  }
  ImGui::EndChild();

}

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
}

void Renderer::renderGrid() {
  Grid& grid = engine_.getGrid();
  std::size_t rows = grid.getGridValues().size() / grid.getWidth();
  std::size_t cols = grid.getWidth();
  std::vector<uint8_t> cells = grid.getGridValues();

  const float cellSize = cell_size_ * zoom_;
  const float grid_w = cols * cellSize;
  const float grid_h = rows * cellSize;
  const float line_thickness = 1.0f;
  const ImU32 gridColor = IM_COL32(50, 50, 50, 255);
  const ImU32 fillColor = IM_COL32(200, 200, 200, 255);

  ImGui::SameLine();
  ImGui::BeginChild("GridPane", ImVec2(grid_w + 2.0f, grid_h + 2.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  {
    ImVec2 p0 = ImGui::GetCursorScreenPos(); // top-left of the canvas
    const ImVec2 size(grid_w, grid_h); 
    const char* id = "##grid";
    ImGui::InvisibleButton(id, size);
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p0, ImVec2(p0.x + size.x, p0.y + size.y), IM_COL32(25, 25, 25, 255));
    dl->AddRect(p0, ImVec2(p0.x + size.x, p0.y + size.y), IM_COL32(130, 130, 130, 255), 0.0f, 0, line_thickness);
    // Draw filled cells
    for (std::size_t r = 0; r < rows; ++r) {
      for (std::size_t c = 0; c < cols; ++c) {
        if (cells[r * cols + c] & 0x03) { // last two bits indicate filled state (2 = marked, 1 = seed)
          ImVec2 a(p0.x + c * cellSize + 1, p0.y + r * cellSize + 1);
          ImVec2 b(p0.x + (c + 1) * cellSize - 1, p0.y + (r + 1) * cellSize - 1);
          dl->AddRectFilled(a, b, fillColor);
        }
      }
    }
    
    // Draw grid lines
    for (std::size_t c = 1; c < cols; ++c) {
      float x = p0.x + c * cellSize;
      dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p0.y + size.y), gridColor, line_thickness);
    }
    for (std::size_t r = 1; r < rows; ++r) {
      float y = p0.y + r * cellSize;
      dl->AddLine(ImVec2(p0.x, y), ImVec2(p0.x + size.x, y), gridColor, line_thickness);
    }

    // Interaction: click/drag to toggle or paint
    if ((hovered || active) && paused_) {
      ImVec2 mp = ImGui::GetIO().MousePos;
      int col = (int)((mp.x - p0.x) / cellSize);
      int row = (int)((mp.y - p0.y) / cellSize);
      if (col >= 0 && col < (int)cols && row >= 0 && row < (int)rows) {
        int idx = row * cols + col;
        // On mouse press: toggle
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
          cells[idx] ^= 1u;
          grid.setCell(col, row, cells[idx]);
        }
        // Drag to paint/erase
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
          bool erase = ImGui::GetIO().KeyShift;
          uint8_t newVal = erase ? 0u : 1u;
          if (cells[idx] != newVal) {
            cells[idx] = newVal;
            grid.setCell(col, row, cells[idx]);
          }
        }
      }
      ImVec2 a(p0.x + col * cellSize + 1,       p0.y + row * cellSize + 1);
      ImVec2 b(p0.x + (col + 1) * cellSize - 1, p0.y + (row + 1) * cellSize - 1);
      dl->AddRect(a, b, IM_COL32(255, 255, 255, 80));
    }
  }
  ImGui::EndChild();
}

Renderer::~Renderer() {
  tearDownImGui();
}

void Renderer::setUpImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForSDLRenderer(window_, sdl_renderer_);
  ImGui_ImplSDLRenderer2_Init(sdl_renderer_);
}

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

void Renderer::renderAbout() {
  // TODO: implement this
}

void Renderer::renderRuleSettings() {
  
}

