#include "erosion.hpp"


uint8_t ErosionRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  return apply(current_state, neighbours);
}

uint8_t ErosionRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (current_state == 0) return 0;
  for (auto neighbour : neighbours) {
    if (neighbour == 0) {
      return 0;
    }
  }
  return current_state;
}

std::string ErosionRule::getName() const {
    return EROSION_RULE_NAME;
}

