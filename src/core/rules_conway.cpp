#include "rules_conway.hpp"

// Core Conway logic:
// Uses only LSB as "alive" flag → allows packing extra data in other bits
uint8_t ConwayRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  std::size_t alive_count = 0;
  uint8_t state = 0; // unused (leftover? could be removed)

  // Count alive neighbors (only LSB matters)
  for (auto neighbour : neighbours) {
    if (neighbour & 0x01) alive_count++;
  }

  // If current cell is alive
  if (current_state & 0x01) {
    // Survives with 2 or 3 neighbors, otherwise dies
    return (alive_count == 2 || alive_count == 3)
      ? (current_state | 0x01)      // keep alive bit
      : (current_state & ~0x01);    // clear alive bit
  } else {
    // Dead cell becomes alive only with exactly 3 neighbors
    return (alive_count == 3)
      ? (current_state | 0x01)
      : (current_state & ~0x01); 
  }
}

std::string ConwayRule::getName() const {
  return CONWAY_RULE_NAME; 
}
