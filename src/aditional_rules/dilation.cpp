#include "dilation.hpp"


uint8_t DilationRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (current_state != 0) return current_state;
  for (auto neighbour : neighbours) {
    if (neighbour != 0) {
      return neighbour;
    }
  }
  return current_state;
}

std::string DilationRule::getName() const {
    return DILATION_RULE_NAME;
}
