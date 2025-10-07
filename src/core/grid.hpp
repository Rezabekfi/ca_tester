#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "rule.hpp"

// Different boundary conditions for the grid
// INFO: Few of those wont be implemented at first (will most likely opt into default which is wrap)
enum class Boundary : uint8_t {
  Wrap, Reflect, Clamp, Zero, One
};

enum class Neighborhood : uint8_t {
  Moore, VonNeumann
};


inline std::size_t idx(std::size_t x, std::size_t y, std::size_t width) {
  return y * width + x;
}

// 2D grid of cells for cellular automata. For now only supports binary states but can be extended later (aslo it is uint_8 so technically not binary already)
class Grid {
public:

  Grid() = default;

  Grid(std::size_t width, std::size_t height, uint8_t default_state = 0, // magic constant take care of it later
       Boundary boundary = Boundary::Wrap, Neighborhood neighborhood = Neighborhood::Moore);

  void step(const Rule& rule);

  void setCell(std::size_t x, std::size_t y, uint8_t state);
  uint8_t getCell(std::size_t x, std::size_t y) const;

  std::vector<uint8_t> getGridValues() const;

  void setGridValues(const std::vector<uint8_t>& new_grid_values);

  std::size_t getWidth() const;

  ~Grid() = default;
  
private:
  std::size_t width_;
  std::size_t height_;
  Boundary boundary_;
  Neighborhood neighborhood_;
  std::vector<uint8_t> cells_;
  std::vector<uint8_t> new_cells_;

  std::vector<uint8_t> getNeighbors(std::size_t x, std::size_t y) const;
};
