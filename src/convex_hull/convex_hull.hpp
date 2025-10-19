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
private:
  bool even_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
  bool odd_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
  bool back_mark(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
  bool exists_oposite_marked_neighbor(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
};
