#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>

class Grid; // fallback forward decl in case grid_fwd not included

enum class Neighborhood : uint8_t;
enum class Boundary : uint8_t;

// Extra info passed into rules that need more than just neighbor values
// Useful for position-aware rules or rules with custom neighborhood sampling
struct RuleContext {
  const Grid& grid;
  std::size_t x;
  std::size_t y;
  Neighborhood neighborhood;
  Boundary boundary;
  std::size_t radius;

  // Sample neighbors around any cell, not only the current one
  std::vector<uint8_t> getNeighbors(std::size_t px, std::size_t py) const;

  // Gets values from a rectangular/edge-like region
  std::vector<uint8_t> getEdgeNeighborhood(std::size_t x1, std::size_t y1,
                                           std::size_t x2, std::size_t y2) const;

  // Same region query, but keeps coordinates instead of cell states
  // This is needed in Convex Hull to check for a distance between certain values
  std::vector<std::pair<int, int>> getEdgeNeighborhoodWithCoordinates(std::size_t x1, std::size_t y1,
                                                           std::size_t x2, std::size_t y2) const;

  // Gets neighborhood coordinates for custom rule logic
  std::vector<std::pair<int, int>> getNeighborhoodWithCoordinates(std::size_t x, std::size_t y) const;

  const Grid& getGrid() const { return grid; }
  
  Neighborhood getNeighborhood() const { return neighborhood; }
  Boundary getBoundary() const { return boundary; }

  // Lets rules temporarily override how they sample neighbors
  void setNeighborhood(Neighborhood n) { neighborhood = n; }
  void setBoundary(Boundary b) { boundary = b; }

  // Radius controls wider-than-default neighborhoods
  // This ended up not being used at all but it might be implemented in the future
  void setRadius(std::size_t r) { radius = r; }
  
  std::size_t getRadius() const { return radius; }

  // Exposes simulation time to time-dependent rules
  std::size_t getIteration() const;
};
