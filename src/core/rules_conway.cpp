#include "rules_conway.hpp"

uint8_t ConwayRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  std::size_t alive_count = 0;
  uint8_t state = 0;
  for (auto neighbour : neighbours) {
    if (neighbour & 0x01) alive_count++;
  }
  if (current_state & 0x01) {
    return (alive_count == 2 || alive_count == 3) ? (current_state | 0x01) : (current_state & ~0x01);
  } else {
    return (alive_count == 3) ? (current_state | 0x01) : (current_state & ~0x01); 
  }
}

std::string ConwayRule::getName() const {
  return CONWAY_RULE_NAME;
}
