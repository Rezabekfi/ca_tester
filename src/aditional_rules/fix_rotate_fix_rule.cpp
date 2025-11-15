#include "fix_rotate_fix_rule.hpp"
#include "core/grid.hpp"

uint8_t FixRotateFix::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  uint8_t result_state = current_state;

  if (state != 2) { // TODO: remove magic number
    result_state = FixingRectangleRule::getInstance().apply(current_state, ctx, neighbours);
  } else {
    result_state = RotationRule::getInstance().apply(current_state, ctx, neighbours);
  }


  if ((ctx.x + ctx.y * ctx.getGrid().getWidth() + 1) == ctx.getGrid().getWidth() * ctx.getGrid().getHeight()) {
    state = step(state);
  }
  return result_state;
}

uint8_t FixRotateFix::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

std::string FixRotateFix::getName() const {
    return FIX_ROTATE_FIX_RULE_NAME;
}
