cmake_minimum_required(VERSION 3.20)
project(ca_min LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ca_core
  src/core/grid.cpp
  src/core/engine.cpp
  src/core/rules_conway.cpp
  src/core/io.cpp
)
target_include_directories(ca_core PUBLIC src)

find_package(SDL2 REQUIRED)
if (TARGET SDL2::SDL2)
  set(SDL2_TARGET SDL2::SDL2)
elseif (TARGET SDL2)         # some FindSDL2 modules expose this name
  set(SDL2_TARGET SDL2)
else()
  message(FATAL_ERROR "SDL2 target not found")
endif()

add_library(imgui STATIC
  third_party/imgui/imgui.cpp
  third_party/imgui/imgui_draw.cpp
  third_party/imgui/imgui_tables.cpp
  third_party/imgui/imgui_widgets.cpp
  third_party/imgui/backends/imgui_impl_sdl2.cpp
  third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
  third_party/imgui/imgui_demo.cpp   # uncomment to include the demo window
)
target_include_directories(imgui PUBLIC
  third_party/imgui
  third_party/imgui/backends
)
target_link_libraries(imgui PUBLIC ${SDL2_TARGET})  # backends need SDL2

add_executable(ca_app
  src/app_sdl/main.cpp
  src/app_sdl/renderer.cpp
)
target_link_libraries(ca_app PRIVATE ca_core ${SDL2_TARGET} imgui)

