#include "dilation.hpp"


// Simple dilation rule that turns on dead cells if they have at least one alive neighbor
uint8_t DilationRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (current_state & 0x03) { // if the cell is already alive, keep it alive
    return current_state;
  } 
  for (auto neighbour : neighbours) {
    if (neighbour & 0x03) { 
      return neighbour;
    }
  }
  return current_state;
}

std::string DilationRule::getName() const {
    return DILATION_RULE_NAME;
}
