#include "fix_rotate_fix_rule.hpp"
#include "core/grid.hpp"

// WARNING: this might no longer work correctly as apply step is now done in parallel across multiple threads and this rule relies on global state to alternate phases

// This rule combines the fixing rectangle and rotation rules in phases to try to fix rectangles, then rotate them, then fix them again
uint8_t FixRotateFix::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  uint8_t result_state = current_state;

  // Phase 1 and 2: apply fixing rectangle to hopefully fill holes and trim excess in rectangles
  if (state != 2) { 
    result_state = FixingRectangleRule::getInstance().apply(current_state, ctx, neighbours);
  } else {
    result_state = RotationRule::getInstance().apply(current_state, ctx, neighbours);
  }


  // change step at the end of the grid update
  if ((ctx.x + ctx.y * ctx.getGrid().getWidth() + 1) == ctx.getGrid().getWidth() * ctx.getGrid().getHeight()) {
    state = step(state);
  }
  return result_state;
}

// not used version
uint8_t FixRotateFix::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

std::string FixRotateFix::getName() const {
    return FIX_ROTATE_FIX_RULE_NAME;
}
