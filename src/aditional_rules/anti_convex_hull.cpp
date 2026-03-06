#include "anti_convex_hull.hpp"
#include "convex_hull/convex_hull.hpp"

uint8_t AntiConvexHull::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  return apply(current_state, neighbours);
}

uint8_t AntiConvexHull::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return (is_seed(current_state) || is_marked(current_state)) ? INSIDE_CELL_VALUE : OUTSIDE_CELL_VALUE;
}

std::string AntiConvexHull::getName() const {
    return ANTI_CONVEX_HULL_RULE_NAME;
}
