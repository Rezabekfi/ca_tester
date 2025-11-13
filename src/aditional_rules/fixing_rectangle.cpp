#include "fixing_rectangle.hpp"
#include <iostream>
#include "core/grid.hpp"

uint8_t FixingRectangleRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  uint8_t result_state = current_state;
  if (dilation_phase_) {
    result_state = DilationRule::getInstance().apply(current_state, neighbours);
  } else {
    result_state = ErosionRule::getInstance().apply(current_state, neighbours);
  }
  const auto& gridVals = ctx.getGrid().getGridValues();
  const std::size_t width = ctx.getGrid().getWidth();
  if ((ctx.x + ctx.y * width + 1) == gridVals.size()) {
    dilation_phase_ = !dilation_phase_;
  }
  return result_state;
}

uint8_t FixingRectangleRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

std::string FixingRectangleRule::getName() const {
    return FIXING_RECTANGLE_RULE_NAME;
}

