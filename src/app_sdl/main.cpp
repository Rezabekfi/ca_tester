#include <SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <stdio.h>
#include "core/engine.hpp"
#include "core/rules_conway.hpp"
#include <iostream>

// for now just a simple window created by chat for testing if setup works
int main(int, char**)
{
  {
    ConwayRule rule;
    Engine engine(100, 100, rule);
    engine.toggleRunning();
    while (engine.getIteration() < 1000) {
      SDL_Delay(100); // Sleep for 100 milliseconds
    }
  }
  return 0;







    // 1. Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("CA App - Hello Window",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!window || !renderer) {
        printf("SDL window/renderer creation failed: %s\n", SDL_GetError());
        return -1;
    }

    // 2. Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // 3. Initialize SDL2 + SDL_Renderer backends for ImGui
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool show_demo_window = true;
    bool show_hello_window = true;
    bool done = false;

    // 4. Main loop
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // 5. Start new ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Show demo window (ImGui built-in)
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Our custom Hello window
        if (show_hello_window) {
            ImGui::Begin("Hello, CA World!", &show_hello_window);
            ImGui::Text("This is your first ImGui window.");
            ImGui::Checkbox("Show Demo Window", &show_demo_window);
            if (ImGui::Button("Close")) show_hello_window = false;
            ImGui::End();
        }

        // 6. Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // background
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // 7. Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

