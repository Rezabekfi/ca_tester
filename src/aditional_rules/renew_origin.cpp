#include "renew_origin.hpp"
#include "convex_hull/convex_hull.hpp"
#include "anti_convex_hull.hpp"


uint8_t RenewOriginRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  return apply(current_state, neighbours);
}

uint8_t RenewOriginRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (is_origin(current_state)) {
    return INSIDE_CELL_VALUE;
  }
  return current_state;
}

std::string RenewOriginRule::getName() const {
    return RENEW_ORIGIN_RULE_NAME;
}
