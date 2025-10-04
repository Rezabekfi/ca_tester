#include "rules_conway.hpp"

uint8_t ConwayRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  std::size_t alive_count = 0;
  uint8_t state = 0;
  for (auto neighbour : neighbours) {
    if (neighbour) alive_count++;
  }
  if (current_state) {
    return (alive_count == 2 || alive_count == 3) ? 1 : 0;
  } else {
    return (alive_count == 3) ? 1 : 0;
  }
}
