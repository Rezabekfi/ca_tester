#include "erosion.hpp"

// Morphological erosion: removes cells that are near empty space
uint8_t ErosionRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if ((current_state & 0x03) == 0) return 0;
  for (auto neighbour : neighbours) {
    if ((neighbour & 0x03) == 0) {
      return current_state & 0xFC; // kill the cell but keep metadata
    }
  }
  return current_state;
}

std::string ErosionRule::getName() const {
    return EROSION_RULE_NAME;
}

