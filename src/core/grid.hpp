#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>
#include <utility>
#include <span>
#include "rule.hpp"

enum class Boundary : uint8_t {
  Wrap, Reflect, Clamp, Zero, One, Count
};
// IDK this looks ugly... TODO: take a look at this
// boundary to string
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

enum class Neighborhood : uint8_t {
  Moore, VonNeumann, Count
};

constexpr std::array<std::pair<int,int>, 8> deltas_moore = {{
  {0,-1}, {-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1}
}};

constexpr std::array<std::pair<int,int>, 4> deltas_vonneumann = {{
  {0,-1}, {-1,0}, {0,1}, {1,0}
}};

inline std::span<const std::pair<int,int>> pick_deltas(Neighborhood n) {
    return (n == Neighborhood::Moore) ? std::span<const std::pair<int,int>>(deltas_moore) : std::span<const std::pair<int,int>>(deltas_vonneumann);
}

// neighborhood to string
inline const char* neighborhoodToString(Neighborhood n) {
  switch (n) {
    case Neighborhood::Moore: return "Moore";
    case Neighborhood::VonNeumann: return "Von Neumann";
    default: return "Unknown";
  }
}

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

  std::vector<uint8_t>& getGridValues();
  const std::vector<uint8_t>& getGridValues() const;


  void setGridValues(const std::vector<uint8_t>& new_grid_values);

  void resize(std::size_t new_width, std::size_t new_height);

  void setBoundary(Boundary boundary);
  void setNeighborhood(Neighborhood neighborhood);

  std::size_t getWidth() const;
  std::size_t getHeight() const;
  Boundary getBoundary() const;
  Neighborhood getNeighborhood() const;

  static std::vector<uint8_t> getNeighborsStatic(const std::vector<uint8_t>& cells, std::size_t x, std::size_t y, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary);

  ~Grid() = default;
  
private:
  std::size_t width_;
  std::size_t height_;
  Boundary boundary_;
  Neighborhood neighborhood_;
  std::vector<uint8_t> cells_;
  std::vector<uint8_t> new_cells_;

};
