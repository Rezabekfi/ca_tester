#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>

class Grid; // fallback forward decl in case grid_fwd not included

enum class Neighborhood : uint8_t;
enum class Boundary : uint8_t;

struct RuleContext {
  const Grid& grid;
  std::size_t x;
  std::size_t y;
  Neighborhood neighborhood;
  Boundary boundary;

  std::vector<uint8_t> getNeighbors(std::size_t px, std::size_t py) const;
  std::vector<uint8_t> getEdgeNeighborhood(std::size_t x1, std::size_t y1,
                                           std::size_t x2, std::size_t y2) const;
  const Grid& getGrid() const { return grid; }
};

