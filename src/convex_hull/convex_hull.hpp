#pragma once
#include "core/rule.hpp"
#include "core/grid.hpp"


constexpr std::string CONVEX_HULL_RULE_NAME = "Convex Hull"; 

class ConvexHull : public Rule {
public:
  
  ConvexHull() = default;
  ~ConvexHull() override = default;

  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;
  
  void calculateDistances(std::vector<uint8_t>& grid, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary) const;
};
