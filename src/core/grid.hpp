#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>
#include <utility>
#include <span>
#include "rule.hpp"

// How edges behave when neighbor lookup goes out of bounds
enum class Boundary : uint8_t {
  Wrap, Reflect, Clamp, Zero, One, Count
};

// Convert boundary enum to UI/debug string
inline const char* boundaryToString(Boundary b) {
  switch (b) {
    case Boundary::Wrap: return "Wrap";
    case Boundary::Reflect: return "Reflect";
    case Boundary::Clamp: return "Clamp";
    case Boundary::Zero: return "Zero";
    case Boundary::One: return "One";
    default: return "Unknown";
  }
}

// Defines which neighbors are considered during rule evaluation
enum class Neighborhood : uint8_t {
  Moore, VonNeumann, Count
};

// Precomputed neighbor offsets (avoids recomputing each step)
constexpr std::array<std::pair<int,int>, 8> deltas_moore = {{
  {0,-1}, {-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1}
}};

constexpr std::array<std::pair<int,int>, 4> deltas_vonneumann = {{
  {0,-1}, {-1,0}, {0,1}, {1,0}
}};

// Returns correct delta set based on neighborhood (hot path, so lightweight)
inline std::span<const std::pair<int,int>> pick_deltas(Neighborhood n) {
    return (n == Neighborhood::Moore) ? std::span<const std::pair<int,int>>(deltas_moore) : std::span<const std::pair<int,int>>(deltas_vonneumann);
}

// Convert neighborhood enum to string (UI/debug)
inline const char* neighborhoodToString(Neighborhood n) {
  switch (n) {
    case Neighborhood::Moore: return "Moore";
    case Neighborhood::VonNeumann: return "Von Neumann";
    default: return "Unknown";
  }
}

// 2D -> 1D index mapping (row-major layout)
inline std::size_t idx(std::size_t x, std::size_t y, std::size_t width) {
  return y * width + x;
}

// Core simulation container: owns state + handles stepping logic
// Note: double buffer (cells_ / new_cells_) is key to avoid in-place corruption during updates
class Grid {
public:

  Grid() = default;

  // Initializes grid with size + default state and simulation settings
  Grid(std::size_t width, std::size_t height, uint8_t default_state = 0, // TODO: default_state is a bit implicit
       Boundary boundary = Boundary::Wrap, Neighborhood neighborhood = Neighborhood::Moore);

  // Advances simulation by one step using provided rule
  // Rule operates per-cell, using neighbors extracted via current settings
  void step(const Rule& rule);

  void setCell(std::size_t x, std::size_t y, uint8_t state);
  uint8_t getCell(std::size_t x, std::size_t y) const;

  // Direct access (use carefully, bypasses abstraction)
  std::vector<uint8_t>& getGridValues();
  const std::vector<uint8_t>& getGridValues() const;

  // Bulk replace grid (caller must ensure correct size)
  void setGridValues(const std::vector<uint8_t>& new_grid_values);

  // Resizes grid 
  void resize(std::size_t new_width, std::size_t new_height);

  // Runtime config changes (affects future steps)
  void setBoundary(Boundary boundary);
  void setNeighborhood(Neighborhood neighborhood);
  void setWidth(std::size_t width);
  void setHeight(std::size_t height);
  void setIteration(std::size_t iteration);

  std::size_t getWidth() const;
  std::size_t getHeight() const;
  std::size_t getIteration() const;
  Boundary getBoundary() const;
  Neighborhood getNeighborhood() const;

  // Static helper so rules / other systems can reuse neighbor logic without Grid instance
  // Writes neighbor states into 'out' (caller provides buffer to avoid reallocs which can be costly)
  static void getNeighborsStatic(const std::vector<uint8_t>& cells, std::size_t x, std::size_t y, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary, std::vector<uint8_t>& out);

  ~Grid() = default;
  
private:
  std::size_t width_;
  std::size_t height_;
  std::size_t iteration_; // tracks simulation progress (useful for UI / debugging)
  Boundary boundary_;
  Neighborhood neighborhood_;

  std::vector<uint8_t> cells_;     // current state
  std::vector<uint8_t> new_cells_; // next state (double buffer)

};
