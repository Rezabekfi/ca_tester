
cmake_minimum_required(VERSION 3.20)
project(ca_min LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Use Homebrew clang++
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++" CACHE FILEPATH "" FORCE)

# macOS target + SDK
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "" FORCE)
execute_process(COMMAND xcrun --show-sdk-path
  OUTPUT_VARIABLE MACOSX_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_OSX_SYSROOT "${MACOSX_SDK_PATH}" CACHE PATH "" FORCE)

# Your private libc++ prefix
set(LIBCXX_PREFIX "/Users/filiprezabek/llvm-libcxx")

# Fail early if it isn't there (helps catch typos / missing install)
if(NOT EXISTS ${LIBCXX_PREFIX}/lib OR NOT EXISTS ${LIBCXX_PREFIX}/include/c++/v1)
  message(FATAL_ERROR "Expected libc++ at ${LIBCXX_PREFIX} but it was not found. Did 'ninja install' complete?")
endif()

# Compile with matching libc++ headers
add_compile_options(
  -nostdinc++
  -isystem ${LIBCXX_PREFIX}/include/c++/v1
  -stdlib=libc++
  -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
)

# Link with your libc++ runtime (NO QUOTES!)
add_link_options(
  -L${LIBCXX_PREFIX}/lib
  -Wl,-rpath,${LIBCXX_PREFIX}/lib
  -lc++
  -lc++abi
  -lunwind
  -stdlib=libc++
  -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}
)

add_library(ca_core
  src/core/grid.cpp
  src/core/engine.cpp
  src/core/rules_conway.cpp
  src/core/io.cpp
  src/core/rule_context.cpp
  src/convex_hull/convex_hull.cpp
  src/aditional_rules/erosion.cpp
  src/aditional_rules/dilation.cpp
  src/aditional_rules/fixing_rectangle.cpp
)
target_include_directories(ca_core PUBLIC src)

find_package(SDL2 REQUIRED)
if (TARGET SDL2::SDL2)
  set(SDL2_TARGET SDL2::SDL2)
elseif (TARGET SDL2)
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
  third_party/imgui/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC
  third_party/imgui
  third_party/imgui/backends
)
target_link_libraries(imgui PUBLIC ${SDL2_TARGET})

add_executable(ca_app
  src/app_sdl/main.cpp
  src/app_sdl/renderer.cpp
)
target_link_libraries(ca_app PRIVATE ca_core ${SDL2_TARGET} imgui)

