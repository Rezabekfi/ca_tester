#include "renew_origin.hpp"
#include "convex_hull/convex_hull.hpp"
#include "anti_convex_hull.hpp"


// context version is not necessary
uint8_t RenewOriginRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  return apply(current_state, neighbours);
}

// This rule checks if the origin flag is set and if so, it revives the cell by setting it to INSIDE_CELL_VALUE for other rules
// INSIDE_CELL_VALUE is basically just a 1 and flag for inside value
uint8_t RenewOriginRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (is_origin(current_state)) {
    return INSIDE_CELL_VALUE; // active and inside
  }
  return current_state;
}

std::string RenewOriginRule::getName() const {
    return RENEW_ORIGIN_RULE_NAME;
}
