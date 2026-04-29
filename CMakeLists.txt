cmake_minimum_required(VERSION 3.20)
project(ca_min LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# SDL2 bundled from source
# ------------------------------------------------------------

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/SDL)

# ------------------------------------------------------------
# Core library
# ------------------------------------------------------------

add_library(ca_core
  src/core/grid.cpp
  src/core/engine.cpp
  src/core/rules_conway.cpp
  src/core/io.cpp
  src/core/rule_context.cpp
  src/convex_hull/convex_hull.cpp
  src/aditional_rules/erosion.cpp
  src/aditional_rules/dilation.cpp
  #src/aditional_rules/fixing_rectangle.cpp
  src/aditional_rules/rotation_rule.cpp
  #src/aditional_rules/fix_rotate_fix_rule.cpp
  src/aditional_rules/wolfram_rules.cpp
  src/aditional_rules/line_completor.cpp
  src/aditional_rules/shape_enforcement_rule.cpp
  src/aditional_rules/edge_detection.cpp
  src/aditional_rules/renew_origin.cpp
  src/aditional_rules/anti_convex_hull.cpp
)

target_include_directories(ca_core PUBLIC src)

# ------------------------------------------------------------
# ImGui
# ------------------------------------------------------------

add_library(imgui STATIC
  third_party/imgui/imgui.cpp
  third_party/imgui/imgui_draw.cpp
  third_party/imgui/imgui_tables.cpp
  third_party/imgui/imgui_widgets.cpp
  third_party/imgui/backends/imgui_impl_sdl2.cpp
  third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
  third_party/imgui/imgui_demo.cpp
)

target_include_directories(imgui PUBLIC
  third_party/imgui
  third_party/imgui/backends
)

target_link_libraries(imgui PUBLIC SDL2::SDL2-static)

# ------------------------------------------------------------
# App
# ------------------------------------------------------------

add_executable(ca_app
  src/app_sdl/main.cpp
  src/app_sdl/renderer.cpp
)

target_link_libraries(ca_app PRIVATE
  ca_core
  imgui
  SDL2::SDL2-static
)

if(TARGET SDL2::SDL2main)
  target_link_libraries(ca_app PRIVATE SDL2::SDL2main)
endif()
