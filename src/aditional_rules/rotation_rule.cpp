#include "rotation_rule.hpp"
#include "core/grid.hpp"
#include <cmath>

// This rule rotates cell states based on their neighbors by looking at a point rotated by a certain degree around a fixed point and copying its state here
// A bit of a cheat since it relies on whole grid access instead of just neighbors but it is still interesting to see how it performs and imitates the rotation effect from the PHD thesis as mentioned in hpp file
uint8_t RotationRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  // find if point at negative rotation_degree exists and is active if it is copy it here
  auto& gridVals = ctx.getGrid().getGridValues();
  const std::size_t width = ctx.getGrid().getWidth();
  const std::size_t height = ctx.getGrid().getHeight();

  auto [rotated_x, rotated_y] = rotate_point(static_cast<int>(ctx.x), static_cast<int>(ctx.y), -rotation_degree_);

  if (rotated_x < 0 || rotated_x >= static_cast<int>(width) || rotated_y < 0 || rotated_y >= static_cast<int>(height)) {
    return 0; // out of bounds TODO: check for boundary conditions here
  }
  uint8_t rotated_state = gridVals[rotated_y * width + rotated_x];
  // TODO: add flag so we can rotate only certain objects (e.g. add index to 1 whole rectangle and then rotating only cells with that index)
  // if (roated_state has correct flag) {return roated_state;} else {return current_state;}

  return rotated_state;
} 

std::pair<int, int> RotationRule::rotate_point(int x, int y, double degree) const {
  int rel_x = x - static_cast<int>(fixed_point_.first);
  int rel_y = y - static_cast<int>(fixed_point_.second);
  int rotated_x = static_cast<int>(std::round(cos(degree * M_PI / 180.0) * rel_x - sin(degree * M_PI / 180.0) * rel_y));
  int rotated_y = static_cast<int>(std::round(sin(degree * M_PI / 180.0) * rel_x + cos(degree * M_PI / 180.0) * rel_y));
  rotated_x += static_cast<int>(fixed_point_.first);
  rotated_y += static_cast<int>(fixed_point_.second);
  return {rotated_x, rotated_y};
}

uint8_t RotationRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

std::string RotationRule::getName() const {
    return ROTATION_RULE_NAME;
}
