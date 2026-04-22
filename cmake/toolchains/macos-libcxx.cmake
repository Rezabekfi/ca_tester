set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++" CACHE FILEPATH "")
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "")
set(CMAKE_OSX_SYSROOT "macosx" CACHE STRING "")

set(LIBCXX_PREFIX "/Users/filiprezabek/llvm-libcxx" CACHE PATH "")

if(NOT EXISTS "${LIBCXX_PREFIX}/lib" OR NOT EXISTS "${LIBCXX_PREFIX}/include/c++/v1")
  message(FATAL_ERROR "Expected libc++ at ${LIBCXX_PREFIX} but it was not found.")
endif()

add_compile_options(
  -nostdinc++
  -isystem ${LIBCXX_PREFIX}/include/c++/v1
  -stdlib=libc++
)

add_link_options(
  -L${LIBCXX_PREFIX}/lib
  -Wl,-rpath,${LIBCXX_PREFIX}/lib
  -lc++
  -lc++abi
  -lunwind
  -stdlib=libc++
)
