#include "anti_convex_hull.hpp"
#include "convex_hull/convex_hull.hpp"

// not using this version
uint8_t AntiConvexHull::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  return apply(current_state, neighbours);
}

// Marks all non-alive cells as outside and rest as inside, this is used for other rules to easily check if a cell is inside or outside the object 
uint8_t AntiConvexHull::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (is_origin(current_state)) return mark_origin(INSIDE_CELL_VALUE); 
  return (is_seed(current_state) || is_marked(current_state)) ? INSIDE_CELL_VALUE : OUTSIDE_CELL_VALUE;
}

std::string AntiConvexHull::getName() const {
    return ANTI_CONVEX_HULL_RULE_NAME;
}
