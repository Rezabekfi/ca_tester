#include "renderer.hpp"
#include <SDL_video.h>
#include <imgui.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL.h>


Renderer::Renderer(Engine& engine, std::size_t cell_size)
  : engine_(engine), cell_size_(cell_size) {
  if (!sdl_init()) {
    throw std::runtime_error("Failed to initialize SDL");
  }
  setUpImGui();
}

void Renderer::render() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    ImGui_ImplSDL2_ProcessEvent(&e);
    if (e.type == SDL_QUIT) {
      exit(0); // TODO: figure out if this should be exit or return or something else
    }
    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
      exit(0);
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

  ImVec2 winSize = ImGui::GetWindowSize();
  ImGui::End();

  ImGui::Render();
  SDL_SetRenderDrawColor(sdl_renderer_, 18, 18, 18, 255);
  SDL_RenderClear(sdl_renderer_);
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdl_renderer_);
  SDL_RenderPresent(sdl_renderer_);
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

    ImGui::SliderFloat("Speed", &speed_from_slider_, 0.0f, 100.0f, "%.1f");
    if (ImGui::Button(paused_ ? "Start" : "Pause")) {
      engine_.toggleRunning();
      paused_ = !paused_;
    }
    ImGui::SameLine();
    if (paused_ && ImGui::Button("Step")) {
      engine_.step();
    } else if (!paused_) {
      ImGui::BeginDisabled();
      ImGui::Button("Step");
      ImGui::EndDisabled();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      paused_ = true;
      engine_.reset();
    }
    ImGui::Separator();
    ImGui::Text("Grid Settings");
    // TODO: add more settings here (rule and square size?)
  }
  ImGui::EndChild();
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
    // TODO: implement grid (might reuse code from main.cpp but could be rewritten)
    
  }

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

