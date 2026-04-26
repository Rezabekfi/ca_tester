#include "example_rule.hpp" // this include is necessary if youre copying the example rule and renaming it make sure to change the include

// Basic rule example:
// Counts alive neighbors and flips the cell if exactly 3 are alive
uint8_t ExampleRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  int alive_neighbors = 0;

  // Only LSB is treated as "alive" consistent with rest of project
  for (uint8_t neighbor : neighbours) {
    if (neighbor & 0x01) {
      alive_neighbors++;
    }
  }

  // Instead of setting state explicitly, this toggles it
  // shows how rules can mutate bits without caring about other flags
  if (alive_neighbors == 3) {
    return current_state ^ 0x01;
  }

  return current_state;
}

// Context version just delegates shows backward compatibility pattern
// If you would like to use context features, implement your logic here and the simple version can be left as is or just put some simple line such as return current_state; you cannot remove it though be wary of that
uint8_t ExampleRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  return apply(current_state, neighbours);
}

// dont forget to change the contant name if you did so previosly
std::string ExampleRule::getName() const {
  return EXAMPLE_RULE_NAME;
}
