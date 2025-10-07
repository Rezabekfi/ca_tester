
// main_sdlrenderer.cpp
#include <SDL.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"  // or *_sdlrenderer3.h if using SDL3
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm> // std::clamp
#include <cmath>     // std::floor


// WARNING: this is just for testing few of the gui concepts

// --- Grid widget: draw + interact
// cells: rows*cols entries (0 or 1). Click toggles, drag paints. Shift+drag erases.
static bool GridWidget(const char* id, int rows, int cols, float cellSize,
                       std::vector<uint8_t>& cells,
                       float line_thickness = 1.0f,
                       ImU32 gridColor = IM_COL32(80, 80, 80, 255),
                       ImU32 fillColor = IM_COL32(60, 170, 255, 255))
{
    IM_ASSERT((int)cells.size() == rows * cols);
    bool changed = false;

    // Layout
    ImGui::Text("%s", id);
    ImVec2 p0 = ImGui::GetCursorScreenPos(); // top-left of the canvas
    const ImVec2 size(cols * cellSize, rows * cellSize);

    // Make a single large invisible button to capture interactions
    ImGui::InvisibleButton(id, size);
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Draw background
    dl->AddRectFilled(p0, ImVec2(p0.x + size.x, p0.y + size.y), IM_COL32(25, 25, 25, 255));
    dl->AddRect(p0, ImVec2(p0.x + size.x, p0.y + size.y), IM_COL32(130, 130, 130, 255), 0.0f, 0, line_thickness);

    // Draw filled cells
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (cells[r * cols + c])
            {
                ImVec2 a(p0.x + c * cellSize + 1,        p0.y + r * cellSize + 1);
                ImVec2 b(p0.x + (c + 1) * cellSize - 1,  p0.y + (r + 1) * cellSize - 1);
                dl->AddRectFilled(a, b, fillColor);
            }
        }
    }

    // Draw grid lines (vertical)
    for (int c = 1; c < cols; ++c)
    {
        float x = p0.x + c * cellSize;
        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p0.y + size.y), gridColor, line_thickness);
    }
    // horizontal
    for (int r = 1; r < rows; ++r)
    {
        float y = p0.y + r * cellSize;
        dl->AddLine(ImVec2(p0.x, y), ImVec2(p0.x + size.x, y), gridColor, line_thickness);
    }

    // Interaction: click/drag to toggle or paint
    if (hovered || active)
    {
        ImVec2 mp = ImGui::GetIO().MousePos;
        int col = (int)((mp.x - p0.x) / cellSize);
        int row = (int)((mp.y - p0.y) / cellSize);
        if (col >= 0 && col < cols && row >= 0 && row < rows)
        {
            int idx = row * cols + col;
            // On mouse press: toggle
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                cells[idx] ^= 1u;
                changed = true;
            }
            // Drag to paint/erase
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && active)
            {
                // Hold Shift to erase instead of paint
                bool erase = ImGui::GetIO().KeyShift;
                uint8_t newVal = erase ? 0u : 1u;
                if (cells[idx] != newVal)
                {
                    cells[idx] = newVal;
                    changed = true;
                }
            }

            // Optional hover highlight
            ImVec2 a(p0.x + col * cellSize + 1,       p0.y + row * cellSize + 1);
            ImVec2 b(p0.x + (col + 1) * cellSize - 1, p0.y + (row + 1) * cellSize - 1);
            dl->AddRect(a, b, IM_COL32(255, 255, 255, 80));
        }
    }

    return changed;
}

int main(int, char**)
{
    // Init SDL (video + gamecontroller if you want nav)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) return 1;

    // Create window + renderer
    SDL_Window* window = SDL_CreateWindow(
        "ImGui + SDL_Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!window || !renderer) return 2;

    // ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // optional
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // enable if you want docking

    // Style
    ImGui::StyleColorsDark();

    // Backend bindings
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer); // or ImGui_ImplSDLRenderer3_Init for SDL3

    bool show_demo = true;
    bool running = true;
    while (running)
    {
        // --- 1) Pump SDL events and feed ImGui
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) running = false;
        }

        // --- 2) Start a new ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();


        // --- In your per-frame UI code (after ImGui::NewFrame()) ---

        // Demo grid state (same as before)
        static int rows = 30, cols = 50;
        static std::vector<uint8_t> cells(rows * cols, 0);

        // Layout: one window, two resizable panes
        ImGui::SetNextWindowSize(ImVec2(1100, 700), ImGuiCond_FirstUseEver);
        ImGui::Begin("Editor + Grid");

        // Splitter state
        static float left_width = 320.0f;     // starting width of the control pane
        static const float min_left = 220.0f; // min size
        static const float min_right = 320.0f;
        static const float splitter_w = 6.0f;

        // Total area inside this window
        ImVec2 avail = ImGui::GetContentRegionAvail();

        // Left pane (controls)
        ImGui::BeginChild("Controls", ImVec2(left_width, 0), true);
        {
            ImGui::TextUnformatted("Simulation Controls");
            ImGui::Separator();

            // Placeholders – wire these to your sim as needed
            static float speed = 1.0f;
            static bool paused = false;
            static int step_iters = 1;
            ImGui::SliderFloat("Speed (x)", &speed, 0.05f, 10.0f, "%.2f");
            ImGui::Checkbox("Paused", &paused);
            ImGui::SliderInt("Step Iters", &step_iters, 1, 50);

            if (ImGui::Button("Step Once")) {
                // TODO: advance simulation by 1 * step_iters
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                std::fill(cells.begin(), cells.end(), 0);
            }

            ImGui::Separator();
            ImGui::TextUnformatted("Grid Settings");
            static int pending_rows = rows, pending_cols = cols;
            bool changed_dims = false;
            changed_dims |= ImGui::InputInt("Rows", &pending_rows);
            changed_dims |= ImGui::InputInt("Cols", &pending_cols);
            if (changed_dims) {
                pending_rows = pending_rows < 1 ? 1 : pending_rows;
                pending_cols = pending_cols < 1 ? 1 : pending_cols;
            }
            if (ImGui::Button("Apply Size")) {
                rows = pending_rows; cols = pending_cols;
                cells.assign(rows * cols, 0);
            }
            ImGui::SameLine();
            if (ImGui::Button("Randomize")) {
                for (auto& v : cells) v = (rand() % 3 == 0) ? 1 : 0;
            }

            ImGui::Separator();
            ImGui::TextUnformatted("Editing");
            ImGui::BulletText("Left-click: toggle");
            ImGui::BulletText("Drag: paint on");
            ImGui::BulletText("Shift+Drag: erase");

            ImGui::Separator();
            ImGui::TextWrapped("Tip: drag the bar on the right to resize panes.");
        }
        ImGui::EndChild();

        // Vertical splitter
        ImGui::SameLine();
        ImGui::InvisibleButton("##vsplit", ImVec2(splitter_w, -1));
        if (ImGui::IsItemActive())
        {
            float delta = ImGui::GetIO().MouseDelta.x;
            left_width = std::clamp(left_width + delta, min_left, avail.x - min_right);
        }
        // Draw splitter line
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 sp = ImGui::GetItemRectMin();
        ImVec2 se = ImGui::GetItemRectMax();
        dl->AddRectFilled(sp, se, IM_COL32(150,150,150,120));
        dl->AddLine(ImVec2(sp.x+splitter_w*0.5f, sp.y),
                    ImVec2(sp.x+splitter_w*0.5f, se.y),
                    IM_COL32(90,90,90,255), 1.0f);

        // Right pane (grid canvas)
        ImGui::SameLine();
        ImGui::BeginChild("GridPane", ImVec2(0, 0), true,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        {
            // Compute cell size to fit the pane; keep square cells
            ImVec2 space = ImGui::GetContentRegionAvail();
            float cellSize = 20.0f; // fallback
            if (rows > 0 && cols > 0) {
                float fitX = (space.x - 2.0f) / cols;
                float fitY = (space.y - 2.0f) / rows;
                cellSize = std::floor(std::clamp(std::min(fitX, fitY), 5.0f, 60.0f));
            }

            // Optionally center the grid in the pane
            float grid_w = cols * cellSize;
            float grid_h = rows * cellSize;
            ImVec2 cur = ImGui::GetCursorPos();
            float offX = std::max(0.0f, (space.x - grid_w) * 0.5f);
            float offY = std::max(0.0f, (space.y - grid_h) * 0.5f);
            ImGui::SetCursorPos(ImVec2(cur.x + offX, cur.y + offY));

            // Draw + interact (uses the GridWidget from the previous message)
            bool changed = GridWidget("##grid", rows, cols, cellSize, cells);

            // (Optional) info overlay
            ImGui::SetCursorPos(ImVec2(8, 8));
            ImGui::BeginChild("overlay", ImVec2(220, 60), true, ImGuiWindowFlags_NoScrollbar);
            ImGui::Text("Cells: %dx%d", rows, cols);
            ImGui::Text("Cell size: %.0f", cellSize);
            if (changed) ImGui::Text("Changed this frame");
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::End();


        // --- 4) Render to SDL_Renderer
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 255); // clear
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


