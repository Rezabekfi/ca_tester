#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "rule_context.hpp"

// Base interface for all CA rules
// Rules are stateless and applied per-cell during Grid::step
class Rule {
public:
  virtual ~Rule() = default;

  // Basic rule: only current state + neighbors
  // Most simple rules (like Game of Life) use this
  // IMPORTANT: only the last bit of current_state and neighbors is guaranteed to be the "alive" flag, other bits can be used for extra data by specific rules (e.g. distance in Convex Hull) few methods tested need to keep them in between rule changes so be careful when using other bits 
  // IMPORTANT: neighbors are provided in a fixed order (e.g. for Moore: N, NE, E, SE, S, SW, W, NW) so rules can rely on this order if needed. Order is defined in Grid via deltas for each neighborhood type.
  virtual uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const = 0;

  // Extended version with extra context (iteration, position, etc.)
  // Default just forwards to simple version so old rules still work
  // Implement this whenever you need more info than just current state + neighbors
  // Note: via this you can break basic CA definitions (locality, etc.) so use this with care
  virtual uint8_t apply(uint8_t current_state, const RuleContext& ctx,
                        const std::vector<uint8_t>& neighbours) const {
    return apply(current_state, neighbours); // fallback keeps backward compatibility
  }

  // Used for UI / rule selection
  virtual std::string getName() const = 0;
};
